#pragma once

#include "InputV4L2Base.hpp"

class InputV4L2UserPtr: public InputV4L2
{
public:
    InputV4L2UserPtr(std::shared_ptr<Config> config, std::shared_ptr<ILogger> logger);
    virtual ~InputV4L2UserPtr();

    virtual void EndRead() override;
    virtual void Close() override;
    
protected:
    virtual void CheckDeviceCapabilities(struct v4l2_capability& cap) override;
    virtual void InitDeviceInternal(unsigned int buffer_size) override;
    virtual bool ReadInternal(InputData& data) override;

private:
    struct buffer* _buffers;
    unsigned int   _nBuffers;

    struct v4l2_buffer _buf;
};