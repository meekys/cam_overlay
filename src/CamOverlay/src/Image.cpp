#include <CamOverlay/Image.hpp>
#include <Core/Common.hpp>

Image::Image(std::shared_ptr<Gl::Texture> texture, VERTEX_T (&verticies)[4], mat4_t matrix)
    : _texture(texture), _matrix(matrix)
{
    _program = std::make_shared<Gl::Program>(ReadFile("shader.vert"), ReadFile("shader.frag"));
    _program->AddUniform("projection_matrix");
    _program->AddAttribute("vertex");
    _program->AddAttribute("uv");
    _program->AddUniform("texture");
    
    _buffer = std::make_shared<Gl::Buffer>(sizeof(verticies), verticies);
}

void Image::Draw()
{
    _program->Use();
    _buffer->Bind();

    int stride = sizeof(float) * 5;

    ::glEnableVertexAttribArray(_program->Parameter("vertex"));
    check();
    ::glVertexAttribPointer(_program->Parameter("vertex"), 3, GL_FLOAT, 0, stride, BUFFER_OFFSET(0));
    check();

    ::glEnableVertexAttribArray(_program->Parameter("uv"));
    check();
    ::glVertexAttribPointer(_program->Parameter("uv"), 2, GL_FLOAT, 0, stride, BUFFER_OFFSET(sizeof(float) * 3));
    check();

    ::glUniformMatrix4fv(_program->Parameter("projection_matrix"), 1, 0, (GLfloat*)&_matrix);
    check();

    _texture->Bind();

    ::glUniform1i(_program->Parameter("texture"), 0); // TEXTURE0
    check();

    ::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    check();

    _texture->Unbind();
}