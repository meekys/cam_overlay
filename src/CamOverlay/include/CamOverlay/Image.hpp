#pragma once

#include <memory>

#include <CamOverlay/IDisplay.hpp>
#include <CamOverlay/Math3d.hpp>
#include <CamOverlay/Config.hpp>

#include <Gl/Program.hpp>
#include <Gl/Texture.hpp>
#include <Gl/Buffer.hpp>

typedef struct
{
    vec3_t  p;
    GLfloat u;
    GLfloat v;
} VERTEX_T;

class Image
{
public:
    Image(std::shared_ptr<Config> config, std::shared_ptr<Gl::Texture> texture, VERTEX_T (&verticies)[4], mat4_t matrix);
    virtual ~Image() = default;

    virtual void Draw();

private:
    std::shared_ptr<Config>      _config;
    std::shared_ptr<Gl::Program> _program;
    std::shared_ptr<Gl::Texture> _texture;
    std::shared_ptr<Gl::Buffer>  _buffer;
    mat4_t                       _matrix;
};