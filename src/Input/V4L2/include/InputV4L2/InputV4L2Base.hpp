#pragma once

#include <cstring>
#include <memory>
#include <linux/videodev2.h>
#include <assert.h>

#include <CamOverlay/IInput.hpp>
#include <CamOverlay/Config.hpp>
#include <Core/ILogger.hpp>
#include <Core/Exceptions/Exception.hpp>
#include <Core/Exceptions/ErrNoException.hpp>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

struct buffer
{
    void   *start;
    size_t  length;
};

class InputV4L2 : public IInput
{
public:
    InputV4L2(std::shared_ptr<Config> config, std::shared_ptr<ILogger> logger);
    virtual ~InputV4L2();

    virtual int getWidth() override { return _width; }
    virtual int getHeight() override { return _height; }

    virtual bool Read(InputData& data) override;

protected:
    virtual void Open();
    virtual void Close();

    int xioctl(int fh, unsigned long int request, void *arg);

    virtual void CheckDeviceCapabilities(struct v4l2_capability& cap) = 0;
    virtual void InitDeviceInternal(unsigned int buffer_size) = 0;
    virtual bool ReadInternal(InputData& data) = 0;

    std::shared_ptr<Config>  _config;
    std::shared_ptr<ILogger> _logger;

    int         _fd;
    int         _width;
    int         _height;

private:
    void OpenDevice();
    void InitDevice();
    void InitRead(unsigned int);
    void InitMMap();
    void InitUserP(unsigned int);
    void CloseDevice();
};