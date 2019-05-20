#pragma once

#include <memory>

#include <Core/ILogger.hpp>
#include <CamOverlay/Image.hpp>
#include <CamOverlay/Config.hpp>
#include <CamOverlay/GlContext.hpp>
#include <CamOverlay/IDisplay.hpp>
#include <CamOverlay/IInput.hpp>

class Overlay
{
public:
    Overlay(std::shared_ptr<GlContext> glContext, std::shared_ptr<Config> config, std::shared_ptr<IDisplay> display, std::shared_ptr<IInput> input);

    void Draw();

    static VERTEX_T ImageQuad[4];

private:
    std::shared_ptr<GlContext> _glContext;
    std::shared_ptr<Config>    _config;
    std::shared_ptr<IDisplay>  _display;
    std::shared_ptr<IInput>    _input;
    std::shared_ptr<Image>     _image;
};