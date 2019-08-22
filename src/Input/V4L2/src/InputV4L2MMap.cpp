#include <sys/mman.h>
#include <assert.h>

#include <InputV4L2/InputV4L2MMap.hpp>

InputV4L2MMap::InputV4L2MMap(std::shared_ptr<Config> config, std::shared_ptr<ILogger> logger)
    : InputV4L2(config, logger),
        _buffers(NULL),
        _nBuffers(0)
{
    _logger->Info("Initialising InputV4L2MMap...");

    CLEAR(_buf);

    Open();
}

InputV4L2MMap::~InputV4L2MMap()
{
    Close();
}

void InputV4L2MMap::CheckDeviceCapabilities(struct v4l2_capability& cap)
{    
    if (!(cap.capabilities & V4L2_CAP_STREAMING))
        throw Exception(_config->dev_name + " does not support streaming i/o");
}

void InputV4L2MMap::InitDeviceInternal(unsigned int /*buffer_size*/)
{
    struct v4l2_requestbuffers req;
    CLEAR(req);

    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(_fd, VIDIOC_REQBUFS, &req))
    {
        if (EINVAL == errno)
            throw Exception(_config->dev_name + " does not support memory mapping");

        throw ErrNoException("VIDIOC_REQBUFS");
    }

    if (req.count < 2)
        throw Exception("Insufficient buffer memory on "s + _config->dev_name);

    _buffers = new buffer[req.count]();

    for (_nBuffers = 0; _nBuffers < req.count; ++_nBuffers)
    {
        struct v4l2_buffer buf;
        CLEAR(buf);

        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = _nBuffers;

        if (-1 == xioctl(_fd, VIDIOC_QUERYBUF, &buf))
            throw ErrNoException("VIDIOC_QUERYBUF");

        _buffers[_nBuffers].length = buf.length;
        _buffers[_nBuffers].start = mmap(
            NULL /* start anywhere */,
            buf.length,
            PROT_READ | PROT_WRITE /* required */,
            MAP_SHARED /* recommended */,
            _fd,
            buf.m.offset);

        if (MAP_FAILED == _buffers[_nBuffers].start)
            throw ErrNoException("mmap");
    }

    for (unsigned int i = 0; i < _nBuffers; ++i)
    {
        struct v4l2_buffer buf;
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (-1 == xioctl(_fd, VIDIOC_QBUF, &buf))
            throw ErrNoException("VIDIOC_QBUF");
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(_fd, VIDIOC_STREAMON, &type))
        throw ErrNoException("VIDIOC_STREAMON");
}

bool InputV4L2MMap::ReadInternal(InputData& data)
{
    assert(_buf.bytesused == 0);

    CLEAR(_buf);
    _buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    _buf.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(_fd, VIDIOC_DQBUF, &_buf)) {
        switch (errno) {
        case EAGAIN:
            return false;

        case EIO:
            /* Could ignore EIO, see spec. */

            /* fall through */

        default:
            throw ErrNoException("VIDIOC_DQBUF");
        }
    }

    assert(_buf.index < _nBuffers);

    data.data = _buffers[_buf.index].start;
    data.size = _buf.bytesused;
    
    return true;
}

void InputV4L2MMap::EndRead()
{
    assert(_buf.bytesused > 0);

    if (-1 == xioctl(_fd, VIDIOC_QBUF, &_buf))
        throw ErrNoException("VIDIOC_QBUF");

    CLEAR(_buf);
}

void InputV4L2MMap::Close()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(_fd, VIDIOC_STREAMOFF, &type))
        throw ErrNoException("VIDIOC_STREAMOFF");

    for (unsigned int i = 0; i < _nBuffers; ++i)
        if (-1 == munmap(_buffers[i].start, _buffers[i].length))
            ErrNoException("munmap");

    delete[] _buffers;

    InputV4L2::Close();
}