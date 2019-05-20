#include <string>

#include <OutputFile/OutputFile.hpp>
#include <Core/Common.hpp>
#include <Core/Exceptions/Exception.hpp>
#include <Core/Exceptions/ErrNoException.hpp>

using namespace std::string_literals;

bool OutputFile::SupportsFormat(std::string /*format*/)
{
    return true;
}

OutputFile::OutputFile(std::shared_ptr<ILogger> logger, std::shared_ptr<Config> config, std::shared_ptr<IInput> input)
    : _logger(logger), _config(config), _input(input)
{
    _logger->Info("Initialising OutputFile...");

    ::time_t rawTime;
    ::time(&rawTime);

    auto timeInfo = *::localtime(&rawTime);
    _baseName = FormatDateTime(timeInfo, "./%Y%m%d%H%M%S");

    auto filename = _baseName + ".txt";

    _file = std::ofstream(filename, std::ios::out);

    if (!_file.good())
        throw ErrNoException("Failed to open "s + filename);

    _file << input->getWidth() << std::endl;
    _file << input->getHeight() << std::endl;
    _file << _config->OutputFormat << std::endl;
}

void OutputFile::Update(void* data, int size)
{
    auto filename = _baseName + "-0.dat";
    _file << filename << std::endl;

    auto file = std::ofstream(filename, std::ios::out | std::ios::binary);

    if (!file.good())
        throw ErrNoException("Failed to open "s + filename);

    file.write((char*)data, size);
}

void OutputFile::Draw()
{
}
