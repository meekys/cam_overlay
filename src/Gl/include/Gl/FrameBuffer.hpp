#pragma once

#include <memory>

#include "GlCommon.hpp"
#include "Texture.hpp"

namespace Gl
{

class FrameBuffer
{
public:
    FrameBuffer(std::shared_ptr<Texture> texture);
    ~FrameBuffer();

    void Bind();
    void Unbind();
    
    operator GLuint() const { return _frameBuffer; }

private:
    GLuint _frameBuffer;
};

} // namespace Gl