#pragma once

#include <memory>

#include <CamOverlay/Config.hpp>
#include <CamOverlay/IDisplay.hpp>
#include <CamOverlay/IInput.hpp>
#include <CamOverlay/Image.hpp>
#include <Gl/Texture.hpp>

class ScaledImage
{
public:
    ScaledImage(std::shared_ptr<Config> config, std::shared_ptr<IDisplay> display, std::shared_ptr<IInput> input, std::shared_ptr<Gl::Texture> texture);
    void Draw();

private:
    static VERTEX_T ImageQuad[4];

    std::shared_ptr<Config>   _config;
    std::shared_ptr<IDisplay> _display;
    std::shared_ptr<IInput>   _input;
    std::shared_ptr<Image>    _image;
};