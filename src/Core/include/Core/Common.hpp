#pragma once

#include <ctime>
#include <string>

extern std::string ReadFile(std::string filename);

extern std::string FormatDateTime(std::tm time, const char* format);