#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <png.h>

#include <Gl/Texture.hpp>

class PngTexture
{
public:
    static std::shared_ptr<Gl::Texture> Create(std::string filename);

private:
    PngTexture(std::string filename);
    ~PngTexture();

    void EnsureFileOpen();
    void ValidateHeader();
    void AllocateLibPng();
    void Read();
    Gl::TextureFormat GetFormat(int colorType);

    static void ErrorCallback(png_structp pngPtr, png_const_charp error_msg);
    static void ReadDataCallback(png_structp pngPtr, png_bytep data, png_size_t length);

    std::string _filename;
    std::ifstream _file;

    png_structp _pngPtr;
    png_infop _infoPtr;

    unsigned int      _width;
    unsigned int      _height;
    Gl::TextureFormat _format;

    png_byte* _imageData;
    png_byte** _rowPointers;
};