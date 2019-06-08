#pragma once

#include <memory>
#include <vector>

#include <CamOverlay/ScaledImage.hpp>
#include <CamOverlay/IOutput.hpp>
#include <Core/ILogger.hpp>
#include <CamOverlay/GlContext.hpp>
#include <CamOverlay/IInput.hpp>
#include <CamOverlay/IDisplay.hpp>
#include <CamOverlay/Config.hpp>

#include <Gl/Texture.hpp>
#include <Gl/FrameBuffer.hpp>
#include <Gl/Program.hpp>

class OutputJpeg : public IOutput
{
public:
    static bool SupportsFormat(std::string format);

    OutputJpeg(std::shared_ptr<ILogger> logger, std::shared_ptr<Config> config, std::shared_ptr<GlContext> glContext, std::shared_ptr<IInput> input, std::shared_ptr<IDisplay> display);

    virtual void Update(void* data, int size) override;
    virtual void Draw() override;

    static VERTEX_T BufferQuad[4];

private:
    void UpdateInput(void* data, int size);
    void DrawToFrameBuffer();

    std::shared_ptr<ILogger>   _logger;
    std::shared_ptr<Config>    _config;
    std::shared_ptr<GlContext> _glContext;
    std::shared_ptr<IInput>    _input;
    std::shared_ptr<IDisplay>  _display;

    std::shared_ptr<Gl::Buffer>               _buffer;
    mat4_t                                    _matrix;
    std::shared_ptr<Gl::FrameBuffer>          _renderBuffer;
    std::shared_ptr<Gl::Program>              _program;
    std::shared_ptr<ScaledImage>              _image;
    std::vector<std::shared_ptr<Gl::Texture>> _textures;
};