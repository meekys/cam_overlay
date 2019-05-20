#include <assert.h>

#include <Gl/Texture.hpp>

using namespace Gl;

Texture::Texture(GLint format, GLint width, GLint height, void* data)
{
    ::glGenTextures(1, &_texture);
    check();

    Bind();

    ::glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    check();

    ::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    ::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    check();

    ::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    check();
}

Texture::~Texture()
{
    ::glDeleteTextures(1, &_texture);
    _texture = 0;
}

void Texture::Bind()
{
    ::glBindTexture(GL_TEXTURE_2D, _texture);
    check();
}

void Texture::Unbind()
{
    ::glBindTexture(GL_TEXTURE_2D, 0);
    check();
}