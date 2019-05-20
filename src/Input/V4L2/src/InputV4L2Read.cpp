#include <unistd.h>

#include <InputV4L2/InputV4L2Read.hpp>

InputV4L2Read::InputV4L2Read(std::shared_ptr<Config> config, std::shared_ptr<ILogger> logger)
    : InputV4L2(config, logger)
{
    _logger->Info("Initialising InputV4L2Read...");

    CLEAR(_buffer);

    Open();
}

InputV4L2Read::~InputV4L2Read()
{
    Close();
}

void InputV4L2Read::CheckDeviceCapabilities(struct v4l2_capability& cap)
{
    if (!(cap.capabilities & V4L2_CAP_READWRITE))
        throw Exception(_config->dev_name + " does not support read i/o");
}

void InputV4L2Read::InitDeviceInternal(unsigned int buffer_size)
{
    _buffer.length = buffer_size;
    _buffer.start = malloc(buffer_size);

    if (!_buffer.start)
        throw std::bad_alloc();
}

bool InputV4L2Read::ReadInternal(InputData& data)
{
    if (-1 == ::read(_fd, _buffer.start, _buffer.length))
    {
        switch (errno)
        {
            case EAGAIN:
                return false;

            case EIO:
                /* Could ignore EIO, see spec. */

                /* fall through */

            default:
                throw ErrNoException("read");
        }
    }

    data.data = _buffer.start;
    data.size = _buffer.length;

    return true;
}

void InputV4L2Read::EndRead()
{
    // NOOP
}

void InputV4L2Read::Close()
{
    ::free(_buffer.start);

    InputV4L2::Close();
}