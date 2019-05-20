#pragma once

#include "GlCommon.hpp"

namespace Gl
{

class Buffer
{
public:
    Buffer(GLuint size, const void* data);
    ~Buffer();

    void Bind();

    operator GLuint() const { return _buffer; }

private:
    GLuint _buffer;
};

} // namespace Gl