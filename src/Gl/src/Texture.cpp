#include <assert.h>

#include <Gl/Texture.hpp>

using namespace Gl;

const TextureFormatInfo Texture::TextureFormats[] = {
  { GL_RED,      GL_RED,          GL_UNSIGNED_BYTE },
  { GL_RGB,      GL_RGB,          GL_UNSIGNED_BYTE },
  { GL_RGBA,     GL_RGBA,         GL_UNSIGNED_BYTE },
  { GL_R8UI,     GL_RED_INTEGER,  GL_UNSIGNED_BYTE },
  { GL_RGB8UI,   GL_RGB_INTEGER,  GL_UNSIGNED_BYTE },
  { GL_RGBA8UI,  GL_RGBA_INTEGER, GL_UNSIGNED_BYTE },
  { GL_R16I,     GL_RED_INTEGER,  GL_SHORT },
  { GL_RGBA16I,  GL_RGBA_INTEGER, GL_SHORT },
  { GL_R16UI,    GL_RED_INTEGER,  GL_UNSIGNED_SHORT },
  { GL_RGBA16UI, GL_RED_INTEGER,  GL_UNSIGNED_SHORT },
  { GL_R32I,     GL_RED_INTEGER,  GL_INT },
  { GL_R32UI,    GL_RED_INTEGER,  GL_UNSIGNED_INT },
  { GL_R32F,     GL_RED,          GL_FLOAT },
  { GL_RGBA32F,  GL_RGBA,         GL_FLOAT },
};

Texture::Texture(TextureFormat format, GLint width, GLint height, void* data)
    : Texture(TextureFormats[format].internal, TextureFormats[format].format, TextureFormats[format].type, width, height, data)
{
}

Texture::Texture(GLint internalFormat, GLint format, GLint type, GLint width, GLint height, void* data)
{
    ::glGenTextures(1, &_texture);
    check();

    Bind();

    ::glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
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

void Texture::Bind(int id)
{
    ::glActiveTexture(GL_TEXTURE0 + id);
    check();

    ::glBindTexture(GL_TEXTURE_2D, _texture);
    check();
}

void Texture::Unbind(int id)
{
    ::glActiveTexture(GL_TEXTURE0 + id);
    check();

    ::glBindTexture(GL_TEXTURE_2D, 0);
    check();
}