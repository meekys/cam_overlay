#include <CamOverlay/PngTexture.hpp>

#include <Core/Exceptions/Exception.hpp>
#include <Core/Exceptions/ErrNoException.hpp>

std::shared_ptr<Gl::Texture> PngTexture::Create(std::string filename)
{
    PngTexture png(filename);

    return std::make_shared<Gl::Texture>(
        png._format,
        png._width,
        png._height,
        png._imageData);
}

PngTexture::PngTexture(std::string filename) :
    _filename(filename),
    _file(std::ifstream(filename, std::ios::in | std::ios::binary)),
    _pngPtr(0),
    _infoPtr(0),
    _width(0),
    _height(0),
    _imageData(NULL),
    _rowPointers(NULL)
{
    EnsureFileOpen();
    ValidateHeader();
    AllocateLibPng();
    Read();
}

PngTexture::~PngTexture()
{
    if (_rowPointers)
        delete[] _rowPointers;

    if (_imageData)
        delete[] _imageData;

    ::png_destroy_read_struct(&_pngPtr, &_infoPtr, (png_infopp)NULL);
}

void PngTexture::EnsureFileOpen()
{
    if (!_file)
        throw ErrNoException("Failed to open "s + _filename);
}

void PngTexture::ValidateHeader()
{
    png_byte header[8];

    _file.read((char*)header, sizeof(header));

    if (!_file.good() || png_sig_cmp(header, 0, 8))
        throw Exception("Invalid PNG header while reading "s + _filename);
}

void PngTexture::AllocateLibPng()
{
    _pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)this, PngTexture::ErrorCallback, NULL);
    if (!_pngPtr)
        throw Exception("png_create_read_struct failed");

    _infoPtr = png_create_info_struct(_pngPtr);
    if (!_infoPtr)
        throw Exception("png_create_info_struct failed");
}

void PngTexture::Read()
{
    png_set_read_fn(_pngPtr, (png_voidp)&_file, PngTexture::ReadDataCallback);

    // Already read the first 8 bytes
    png_set_sig_bytes(_pngPtr, _file.tellg());

    png_read_info(_pngPtr, _infoPtr);

    int bitDepth, colorType;
    png_get_IHDR(_pngPtr, _infoPtr, &_width, &_height, &bitDepth, &colorType, NULL, NULL, NULL);

    if (bitDepth != 8)
        throw Exception("Unsupported bit depth "s + std::to_string(bitDepth) + ", must be 8 while reading " + _filename);

    _format = GetFormat(colorType);

    png_read_update_info(_pngPtr, _infoPtr);

    int rowBytes = png_get_rowbytes(_pngPtr, _infoPtr);
    rowBytes += 3 - ((rowBytes - 1) % 4);

    _imageData = new png_byte[rowBytes * _height * sizeof(png_byte) + 15];
    _rowPointers = new png_byte*[_height * sizeof(png_byte *)];

    for (unsigned int i = 0; i < _height; i++)
        _rowPointers[_height - 1 - i] = _imageData + i * rowBytes;

    png_read_image(_pngPtr, _rowPointers);
}

void PngTexture::ErrorCallback(png_structp pngPtr, png_const_charp error_msg)
{
    auto filename = ((PngTexture*)png_get_error_ptr(pngPtr))->_filename;
    throw Exception("Error reading PNG "s + filename + ": " + error_msg);
}

void PngTexture::ReadDataCallback(png_structp pngPtr, png_bytep data, png_size_t length)
{
    auto file = (std::istream*)png_get_io_ptr(pngPtr);
    file->read((char*)data, length);
}

Gl::TextureFormat PngTexture::GetFormat(int colorType)
{
    switch(colorType)
    {
        case PNG_COLOR_TYPE_RGB:
            return Gl::_8_3;

        case PNG_COLOR_TYPE_RGB_ALPHA:
            return Gl::_8_4;
    }

    throw Exception("Unsupported libpng colour type "s + std::to_string(colorType) + " while reading " + _filename);
}