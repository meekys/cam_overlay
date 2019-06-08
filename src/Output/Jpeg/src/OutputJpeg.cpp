#include <string>

#include <OutputJpeg/OutputJpeg.hpp>
#include <OutputJpeg/JpegDecompress.hpp>
#include <Core/Common.hpp>
#include <Core/Exceptions/Exception.hpp>

using namespace std::string_literals;

VERTEX_T OutputJpeg::BufferQuad[4] =
{ //   x,     y,    z,        u,    v
    {{-1.0f,  1.0f, 0.0f},    0.0f, 1.0f}, // Top left
    {{-1.0f, -1.0f, 0.0f},    0.0f, 0.0f}, // Bottom left
    {{ 1.0f,  1.0f, 0.0f},    1.0f, 1.0f}, // Top right
    {{ 1.0f, -1.0f, 0.0f},    1.0f, 0.0f}  // Bottom right
};

bool OutputJpeg::SupportsFormat(std::string format)
{
    return format == "JPEG" || format == "MJPG";
}

OutputJpeg::OutputJpeg(std::shared_ptr<ILogger> logger, std::shared_ptr<Config> config, std::shared_ptr<GlContext> glContext, std::shared_ptr<IInput> input, std::shared_ptr<IDisplay> display)
    : _logger(logger), _config(config), _glContext(glContext), _input(input), _display(display)
{
    _logger->Info("Initialising OutputJpeg...");

    // _texture = std::make_shared<Gl::Texture>(
    //     GL_RGBA,
    //     _input->getWidth() / 2,
    //     _input->getHeight(),
    //     nullptr);
    // ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // check();

    _matrix = m4_identity();

    _program = std::make_shared<Gl::Program>(
        ReadFile(_config->AppPath + "/res/shader.vert"),
        ReadFile(_config->AppPath + "/res/shader-JPEG.frag"));
    _program->AddUniform("projection_matrix");
    _program->AddAttribute("vertex");
    _program->AddAttribute("uv");
    _program->AddUniform("textureY");
    _program->AddUniform("textureU");
    _program->AddUniform("textureV");

    _buffer = std::make_shared<Gl::Buffer>(sizeof(BufferQuad), BufferQuad);

    auto texture = std::make_shared<Gl::Texture>(Gl::_8_4, _input->getWidth(), _input->getHeight(), nullptr);

    _renderBuffer = std::make_shared<Gl::FrameBuffer>(texture);

    _image = std::make_shared<ScaledImage>(_config, _display, _input, texture);
}

void OutputJpeg::UpdateInput(void* data, int size)
{
    auto jpeg = JpegDecompress();
    jpeg.SetMemSrc(data, size);
    jpeg.ReadHeader();

    if (jpeg.components.size() != 3)
        throw Exception("Expected JPEG with 3 channels");

    jpeg.DecodeRawData();

    if (_textures.size() == 0)
    {
        // We assume each JPEG in the stream has the same frame component sizes
        for (unsigned int i = 0; i < jpeg.components.size(); i++)
        {
            auto comp = jpeg.components[i];
            _textures.push_back(std::make_shared<Gl::Texture>(Gl::_8_1, comp.width, comp.height, nullptr));

            ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            check();
        }
    }

    assert(_textures.size() == jpeg.components.size());

    for (unsigned int i = 0; i < jpeg.components.size(); i++)
    {
        auto comp = jpeg.components[i];

        _textures[i]->Bind();

        ::glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0,
            0,
            comp.width,
            comp.height,
            Gl::Texture::TextureFormats[Gl::_8_1].format,
            Gl::Texture::TextureFormats[Gl::_8_1].type,
            comp.data.data());
        check();

        _textures[i]->Unbind();
    }
}

void OutputJpeg::Update(void* data, int size)
{
    UpdateInput(data, size);
    DrawToFrameBuffer();
}

void OutputJpeg::DrawToFrameBuffer()
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
    for (unsigned int i = 0; i < _textures.size(); i++)
        _textures[i]->Bind(i);

    ::glUniform1i(_program->Parameter("textureY"), 0); // TEXTURE0
    ::glUniform1i(_program->Parameter("textureU"), 1); // TEXTURE0
    ::glUniform1i(_program->Parameter("textureV"), 2); // TEXTURE0
    check();

    ::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    for (unsigned int i = 0; i < _textures.size(); i++)
        _textures[i]->Unbind(i);

    _renderBuffer->Unbind();
}

void OutputJpeg::Draw()
{
    _image->Draw();
}