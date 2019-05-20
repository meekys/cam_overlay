#include <assert.h>
#include <vector>

#include <Core/Exceptions/Exception.hpp>
#include <Gl/Shader.hpp>

using namespace Gl;

Shader::Shader(std::string source, GLenum shaderType)
{
    _shader = ::glCreateShader(shaderType);
    check();
    assert(_shader != 0);

    const char* sourcePtr = source.c_str();
    ::glShaderSource(_shader, 1, (const GLchar **)&sourcePtr, 0);
    ::glCompileShader(_shader);
    check();

    GLint isCompiled;
    ::glGetShaderiv(_shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled != GL_TRUE)
        ShaderException();
}

Shader::~Shader()
{
    ::glDeleteShader(_shader);
    _shader = 0;
}

void Shader::ShaderException()
{
    GLint maxLength = 0;
    ::glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<GLchar> infoLog(maxLength);
	::glGetShaderInfoLog(_shader, maxLength, &maxLength, &infoLog[0]);

    throw Exception(&infoLog[0]);
}