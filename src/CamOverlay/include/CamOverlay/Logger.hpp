#pragma once

#include <Core/ILogger.hpp>

class Logger : public ILogger
{
public:
    void Error(const char* format, ...) override;
    void Warn(const char* format, ...) override;
    void Info(const char* format, ...) override;

private:
    void Log(int level, const char* format, va_list args);
};