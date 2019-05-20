#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <InputV4L2/InputV4L2Base.hpp>

using namespace std::string_literals;

InputV4L2::InputV4L2(std::shared_ptr<Config> config, std::shared_ptr<ILogger> logger)
    : _config(config), _logger(logger), _fd(-1)
{
}

InputV4L2::~InputV4L2()
{
}

int InputV4L2::xioctl(int fh, unsigned long int request, void *arg)
{
    int r;

    do
    {
        r = ioctl(fh, request, arg);
    }
    while (-1 == r && EINTR == errno);

    return r;
}

void InputV4L2::Open()
{
    OpenDevice();
    InitDevice();
}

void InputV4L2::OpenDevice()
{
    struct ::stat st;

    if (-1 == stat(_config->dev_name.c_str(), &st))
        throw ErrNoException("Cannot identify '"s + _config->dev_name + "'");

    if (!S_ISCHR(st.st_mode))
        throw Exception(_config->dev_name + " is no device");

    _fd = ::open(_config->dev_name.c_str(), O_RDWR /* required */ | O_NONBLOCK, 0);

    if (-1 == _fd)
        throw Exception("Cannot open '%s'"s + _config->dev_name);

    _logger->Info("Opened device '%s'", _config->dev_name.c_str());
}

void InputV4L2::InitDevice()
{
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;

    if (-1 == xioctl(_fd, VIDIOC_QUERYCAP, &cap))
    {
        if (EINVAL == errno)
            throw Exception(_config->dev_name + " is not a V4L2 device");

        throw ErrNoException("VIDIOC_QUERYCAP");
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
        throw Exception(_config->dev_name + " is not a video capture device");

    CheckDeviceCapabilities(cap);

    /* Select video input, video standard and tune here. */
    CLEAR(cropcap);

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (0 == xioctl(_fd, VIDIOC_CROPCAP, &cropcap))
    {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect; /* reset to default */

        if (-1 == xioctl(_fd, VIDIOC_S_CROP, &crop))
        {
            switch (errno)
            {
                case EINVAL:
                    /* Cropping not supported. */
                    break;
                default:
                    /* Errors ignored. */
                    break;
            }
        }
    }
    else
    {
        /* Errors ignored. */
    }

    CLEAR(fmt);

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == xioctl(_fd, VIDIOC_G_FMT, &fmt))
        throw ErrNoException("VIDIOC_G_FMT");

    static int format[2];
    CLEAR(format);
    format[0] = fmt.fmt.pix.pixelformat;

    _width = fmt.fmt.pix.width;
    _height = fmt.fmt.pix.height;
    _config->OutputFormat = (char*)format;

    InitDeviceInternal(fmt.fmt.pix.sizeimage);
}

bool InputV4L2::Read(InputData& data)
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_fd, &fds);

    /* Timeout. */
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    int r = ::select(_fd + 1, &fds, NULL, NULL, &tv);

    if (-1 == r)
    {
        if (EINTR == errno)
            return false;

        throw Exception("select");
    }

    if (0 == r)
        throw ErrNoException("select timeout");

    return ReadInternal(data);
}

void InputV4L2::Close()
{
    if (-1 == ::close(_fd))
        throw ErrNoException("close");

    _fd = -1;
}