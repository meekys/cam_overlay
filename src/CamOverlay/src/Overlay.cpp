#include <CamOverlay/Overlay.hpp>

#include <CamOverlay/PngTexture.hpp>

VERTEX_T Overlay::ImageQuad[4] =
{   // x,     y,     z,     u,    v
    {{-1.0f,  1.0f,  2.0f}, 0.0f, 1.0f}, // Top left
    {{-1.0f, -1.0f,  1.0f}, 0.0f, 0.0f}, // Bottom left
    {{ 1.0f,  1.0f,  2.0f}, 1.0f, 1.0f}, // Top right
    {{ 1.0f, -1.0f,  1.0f}, 1.0f, 0.0f}  // Bottom right
};

Overlay::Overlay(std::shared_ptr<GlContext> glContext, std::shared_ptr<Config> config, std::shared_ptr<IDisplay> display, std::shared_ptr<IInput> input)
    : _glContext(glContext), _config(config), _display(display), _input(input)
{
    auto texture = PngTexture::Create(_config->AppPath + "/res/overlay.png");

    auto aspect = (float)_display->getWidth() / _display->getHeight();

    auto perspective = m4_perspective2(90.0f, aspect, -1.0f, 1.0f);

    if (!_config->stretch)
        aspect = (float)_input->getWidth() / _input->getHeight();

    auto matrix = m4_mul(perspective, m4_scaling(vec3(aspect, 1, 1)));

    _image = std::make_shared<Image>(_config, texture, ImageQuad, matrix);
}

void Overlay::Draw()
{
    _image->Draw();
}