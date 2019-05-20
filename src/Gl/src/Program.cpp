#include <assert.h>
#include <vector>

#include <Core/Exceptions/Exception.hpp>
#include <Gl/Program.hpp>
#include <Gl/Shader.hpp>

using namespace Gl;

Program::Program(std::string vertex_source, std::string fragment_source)
{
    auto vertex = Shader(vertex_source, GL_VERTEX_SHADER);
    auto fragment = Shader(fragment_source, GL_FRAGMENT_SHADER);

    _program = ::glCreateProgram();
    assert(_program != 0);

    ::glAttachShader(_program, (GLuint)vertex);
    check();

    ::glAttachShader(_program, fragment);
    check();

    ::glLinkProgram(_program);
    check();

    GLint isCompiled;
    ::glGetProgramiv(_program, GL_LINK_STATUS, &isCompiled);
    if (isCompiled != GL_TRUE)
        ProgramException();
}

Program::~Program()
{
    ::glDeleteProgram(_program);
    _program = 0;
}

void Program::Use()
{
    ::glUseProgram(_program);
    check();
}

void Program::ProgramException()
{
    GLint maxLength = 0;
    ::glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<GLchar> infoLog(maxLength);
	::glGetProgramInfoLog(_program, maxLength, &maxLength, &infoLog[0]);

    throw Exception(&infoLog[0]);
}

void Program::AddAttribute(std::string name)
{
    auto attribute = ::glGetAttribLocation(_program, name.c_str());

    _parameters[name] = attribute;
}

void Program::AddUniform(std::string name)
{
    auto uniform = ::glGetUniformLocation(_program, name.c_str());

    _parameters[name] = uniform;
}
