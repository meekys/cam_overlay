#include <string>

#include <OutputShader/OutputShader.hpp>
#include <Core/Common.hpp>

using namespace std::string_literals;

VERTEX_T OutputShader::BufferQuad[4] =
{ //   x,     y,    z,        u,    v
    {{-1.0f,  1.0f, 0.0f},    0.0f, 1.0f}, // Top left
    {{-1.0f, -1.0f, 0.0f},    0.0f, 0.0f}, // Bottom left
    {{ 1.0f,  1.0f, 0.0f},    1.0f, 1.0f}, // Top right
    {{ 1.0f, -1.0f, 0.0f},    1.0f, 0.0f}  // Bottom right
};

bool OutputShader::SupportsFormat(std::string /*format*/)
{
    return true;
}

OutputShader::OutputShader(std::shared_ptr<ILogger> logger, std::shared_ptr<Config> config, std::shared_ptr<GlContext> glContext, std::shared_ptr<IInput> input, std::shared_ptr<IDisplay> display)
    : _logger(logger), _config(config), _glContext(glContext), _input(input), _display(display)
{
    _logger->Info("Initialising OutputShader...");

    _texture = std::make_shared<Gl::Texture>(
        GL_RGBA,
        _input->getWidth() / 2,
        _input->getHeight(),
        nullptr);

    _matrix = m4_identity();

    _program = std::make_shared<Gl::Program>(
        ReadFile(_config->AppPath + "/res/shader.vert"),
        ReadFile(_config->AppPath + "/res/shader-"s + _config->OutputFormat + ".frag"));
    _program->AddUniform("projection_matrix");
    _program->AddAttribute("vertex");
    _program->AddAttribute("uv");
    _program->AddUniform("texture");
    _program->AddUniform("width");

    _buffer = std::make_shared<Gl::Buffer>(sizeof(BufferQuad), BufferQuad);

    auto texture = std::make_shared<Gl::Texture>(GL_RGBA, _input->getWidth(), _input->getHeight(), nullptr);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    check();

    _renderBuffer = std::make_shared<Gl::FrameBuffer>(texture);

    _image = std::make_shared<ScaledImage>(_config, _display, _input, texture);
}

void OutputShader::UpdateInput(void* data)
{
    _texture->Bind();

    ::glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0,
        0,
        _input->getWidth() / 2,
        _input->getHeight(),
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data);

    _texture->Unbind();
}

void OutputShader::Update(void* data, int /*size*/)
{
    UpdateInput(data);
    DrawToFrameBuffer();
}

void OutputShader::DrawToFrameBuffer()
{
    _renderBuffer->Bind();
    ::glViewport(0, 0, _input->getWidth(), _input->getHeight());
    ::glClear(GL_COLOR_BUFFER_BIT);

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

    ::glUniform1i(_program->Parameter("width"), _input->getWidth());
    check();

    _texture->Bind();

    ::glUniform1i(_program->Parameter("texture"), 0); // TEXTURE0
    check();

    ::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    _texture->Unbind();

    _renderBuffer->Unbind();
}

void OutputShader::Draw()
{
    _image->Draw();
}