#pragma once

#include <string>

#include "GlCommon.hpp"

namespace Gl
{

class Shader
{
public:
    Shader(std::string source, GLenum type);
    ~Shader();

    operator GLuint() const { return _shader; }

private:
    void ShaderException();

    GLuint _shader;
};

} // namespace Gl