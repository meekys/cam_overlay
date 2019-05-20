#pragma once

#include <string>
#include <map>

#include "GlCommon.hpp"

namespace Gl
{

class Program
{
public:
    Program(std::string vertex, std::string fragment);
    ~Program();
    
    void Use();

    void AddAttribute(std::string name);
    void AddUniform(std::string name);

    GLuint Parameter(std::string name) { return _parameters[name]; }
    GLuint Parameter(const char* name) { return _parameters[name]; }

    operator GLuint() const { return _program; }
    GLuint& operator[](std::string name) { return _parameters[name]; }
    GLuint& operator[](const char* name) { return _parameters[name]; }

private:
    GLuint CreateShader(std::string source, GLenum shaderType);
    void ProgramException();
    
    GLuint _program;
    std::map<std::string, GLuint> _parameters;
};

} // namespace Gl