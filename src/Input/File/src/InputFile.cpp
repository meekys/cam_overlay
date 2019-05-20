#include <fstream>

#include <InputFile/InputFile.hpp>
#include <Core/Exceptions/ErrNoException.hpp>

InputFile::InputFile(std::shared_ptr<ILogger> logger, std::shared_ptr<Config> config)
: _logger(logger), _config(config), _width(0), _height(0)
{
    _logger->Info("Initialising InputFile...");

    auto file = std::ifstream(_config->dev_name);

    if (!file.good())
        throw ErrNoException("Failed to open "s + _config->dev_name);

    file >> _width >> _height >> _config->OutputFormat >> _filename;
}

void InputFile::ReadFile()
{
    auto file = std::ifstream(_filename, std::ios::in | std::ios::binary | std::ios::ate);

    if (!file.good())
        throw ErrNoException("Failed to open "s + _filename);

    auto size = file.tellg();
    file.seekg(0, std::ios::beg);

    _buffer = std::vector<char>(size);
    if (!file.read(_buffer.data(), size))
        throw ErrNoException("Failed to read "s + _filename);
}

bool InputFile::Read(InputData& data)
{
    if (_buffer.empty())
        ReadFile();

    data.data = &_buffer[0];
    data.size = _buffer.size();

    return true;
}