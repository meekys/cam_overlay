#include <fstream>
#include <sstream>

#include <Core/Common.hpp>
#include <Core/Exceptions/ErrNoException.hpp>

extern std::string ReadFile(std::string filename)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);

    if (!file)
        throw ErrNoException();

    std::ostringstream contents;
    contents << file.rdbuf();

    return contents.str();
}

extern std::string FormatDateTime(std::tm time, const char* format)
{
    char buffer[128];

    if (std::strftime(buffer, sizeof(buffer), format, &time))
        return buffer;

    throw std::invalid_argument("format");
}