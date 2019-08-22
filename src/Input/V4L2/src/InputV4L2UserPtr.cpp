#include <assert.h>

#include <InputV4L2/InputV4L2UserPtr.hpp>

InputV4L2UserPtr::InputV4L2UserPtr(std::shared_ptr<Config> config, std::shared_ptr<ILogger> logger)
    : InputV4L2(config, logger),
        _buffers(NULL),
        _nBuffers(0)
{
    _logger->Info("Initialising InputV4L2UserPtr...");

    CLEAR(_buf);

    Open();
}

InputV4L2UserPtr::~InputV4L2UserPtr()
{
    Close();
}

void InputV4L2UserPtr::CheckDeviceCapabilities(struct v4l2_capability& cap)
{
    if (!(cap.capabilities & V4L2_CAP_STREAMING))
        throw Exception(_config->dev_name + " does not support streaming i/o");
}

void InputV4L2UserPtr::InitDeviceInternal(unsigned int buffer_size)
{
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count  = 4;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_USERPTR;

    if (-1 == xioctl(_fd, VIDIOC_REQBUFS, &req))
    {
        if (EINVAL == errno)
            throw Exception(_config->dev_name + " does not support user pointer i/o");

        throw ErrNoException("VIDIOC_REQBUFS");
    }

    _buffers = new buffer[4]();

    for (_nBuffers = 0; _nBuffers < 4; ++_nBuffers)
    {
        _buffers[_nBuffers].length = buffer_size;
        _buffers[_nBuffers].start = malloc(buffer_size);

        if (!_buffers[_nBuffers].start)
            throw std::bad_alloc();
    }

    for (unsigned int i = 0; i < _nBuffers; ++i) {
        struct v4l2_buffer buf;
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_USERPTR;
        buf.index = i;
        buf.m.userptr = (unsigned long)_buffers[i].start;
        buf.length = _buffers[i].length;

        if (-1 == xioctl(_fd, VIDIOC_QBUF, &buf))
            throw ErrNoException("VIDIOC_QBUF");
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(_fd, VIDIOC_STREAMON, &type))
        throw ErrNoException("VIDIOC_STREAMON");
}

bool InputV4L2UserPtr::ReadInternal(InputData& data)
{
    assert(_buf.bytesused == 0);
    CLEAR(_buf);

    _buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    _buf.memory = V4L2_MEMORY_USERPTR;

    if (-1 == xioctl(_fd, VIDIOC_DQBUF, &_buf))
    {
        switch (errno)
        {
            case EAGAIN:
                return false;

            case EIO:
                /* Could ignore EIO, see spec. */

                /* fall through */

            default:
                throw ErrNoException("VIDIOC_DQBUF");
        }
    }

    unsigned int i;
    for (i = 0; i < _nBuffers; ++i)
        if (_buf.m.userptr == (unsigned long)_buffers[i].start && _buf.length == _buffers[i].length)
            break;

    assert(i < _nBuffers);

    data.data = (void *)_buf.m.userptr;
    data.size = _buf.bytesused;
        
    return true;
}

void InputV4L2UserPtr::EndRead()
{
    assert(_buf.bytesused > 0);

    if (-1 == xioctl(_fd, VIDIOC_QBUF, &_buf))
        throw ErrNoException("VIDIOC_QBUF");

    CLEAR(_buf);
}

void InputV4L2UserPtr::Close()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(_fd, VIDIOC_STREAMOFF, &type))
        throw ErrNoException("VIDIOC_STREAMOFF");

    for (unsigned int i = 0; i < _nBuffers; ++i)
        ::free(_buffers[i].start);

    delete[] _buffers;

    InputV4L2::Close();
}