#pragma once

#include <string>

#include "GlCommon.hpp"

namespace Gl
{

enum TextureFormat {
    _8_1,
    _8_3,
    _8_4
};

struct TextureFormatInfo {
    GLint internal;
    GLenum format;
    GLenum type;
};

class Texture
{
public:
    const static TextureFormatInfo TextureFormats[];

    Texture(TextureFormat format, GLint width, GLint height, void* data);
    Texture(GLint internalFormat, GLint format, GLint type, GLint width, GLint height, void* data);
    ~Texture();

    void Bind(int id = 0);
    void Unbind(int id = 0);

    operator GLuint() const { return _texture; }

private:
    GLuint _texture;
};

} // namespace Gl