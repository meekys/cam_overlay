#include <CamOverlay/ScaledImage.hpp>

VERTEX_T ScaledImage::ImageQuad[4] =
{ //   x,     y,    z,        u,    v
    {{-1.0f,  1.0f, 0.0f},    0.0f, 0.0f}, // Top left
    {{-1.0f, -1.0f, 0.0f},    0.0f, 1.0f}, // Bottom left
    {{ 1.0f,  1.0f, 0.0f},    1.0f, 0.0f}, // Top right
    {{ 1.0f, -1.0f, 0.0f},    1.0f, 1.0f}  // Bottom right
};

ScaledImage::ScaledImage(std::shared_ptr<Config> config, std::shared_ptr<IDisplay> display, std::shared_ptr<IInput> input, std::shared_ptr<Gl::Texture> texture)
    : _config(config), _display(display), _input(input)
{
    auto aspect = (float)_display->getWidth() / _display->getHeight();

    auto ortho = m4_ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

    if (!_config->stretch)
        aspect = (float)_input->getWidth() / _input->getHeight();

    auto matrix = m4_mul(ortho, m4_scaling(vec3(aspect, 1, 1)));

    if (_config->rotate)
        matrix = m4_mul(matrix, m4_rotation_z(M_PI)); // Rotate 180 degrees

    if (_config->flip_horizontal)
        matrix = m4_mul(matrix, m4_scaling(vec3(-1, 1, 1)));

    if (_config->flip_vertical)
        matrix = m4_mul(matrix, m4_scaling(vec3(1, -1, 1)));

    _image = std::make_shared<Image>(_config, texture, ImageQuad, matrix);
}

void ScaledImage::Draw()
{
    _image->Draw();
}