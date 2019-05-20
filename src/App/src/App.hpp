#pragma once

#include <memory>

#include <Core/ILogger.hpp>
#include <CamOverlay/GlContext.hpp>
#include <CamOverlay/Config.hpp>
#include <CamOverlay/IDisplay.hpp>
#include <CamOverlay/IInput.hpp>
#include <CamOverlay/IOutput.hpp>
#include <CamOverlay/Overlay.hpp>

class App
{
public:
    App(
        std::shared_ptr<ILogger> logger,
        std::shared_ptr<Config> config,
        std::shared_ptr<GlContext> glContext,
        std::shared_ptr<IDisplay> display,
        std::shared_ptr<IInput> input,
        std::shared_ptr<IOutput> output,
        std::shared_ptr<Overlay> overlay
    ) :
        _logger(logger),
        _config(config),
        _glContext(glContext),
        _display(display),
        _input(input),
        _output(output),
        _overlay(overlay)
    {}

    void Run();

private:
    std::shared_ptr<ILogger>   _logger;
    std::shared_ptr<Config>    _config;
    std::shared_ptr<GlContext> _glContext;
    std::shared_ptr<IDisplay>  _display;
    std::shared_ptr<IInput>    _input;
    std::shared_ptr<IOutput>   _output;
    std::shared_ptr<Overlay>   _overlay;
};