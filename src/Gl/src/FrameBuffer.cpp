#include <Gl/FrameBuffer.hpp>

using namespace Gl;

FrameBuffer::FrameBuffer(std::shared_ptr<Texture> texture)
{
    ::glGenFramebuffers(1, &_frameBuffer);
    check();

    Bind();

    ::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texture, 0);
    check();

    Unbind();
}

FrameBuffer::~FrameBuffer()
{
    ::glDeleteFramebuffers(1, &_frameBuffer);
    _frameBuffer = 0;
}

void FrameBuffer::Bind()
{
    ::glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    check();
}

void FrameBuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    check();
}