#pragma once

#include <string>

#include "GlCommon.hpp"

namespace Gl
{

class Texture
{
public:  
    Texture(GLint format, GLint width, GLint height, void* data);
    ~Texture();

    void Bind();
    void Unbind();

    operator GLuint() const { return _texture; }

private:
    GLuint _texture;
};

} // namespace Gl