#include <assert.h>

#include <Gl/Buffer.hpp>

using namespace Gl;

Buffer::Buffer(GLuint size, const void* data)
{
    ::glGenBuffers(1, &_buffer);
    check();

    Bind();

    ::glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    check();
}

Buffer::~Buffer()
{
    ::glDeleteBuffers(1, &_buffer);
    _buffer = 0;
}

void Buffer::Bind()
{
    ::glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    check();
}