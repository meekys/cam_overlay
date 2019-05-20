#pragma once

#include <stdarg.h> 

class ILogger
{
public:
    virtual ~ILogger() = default;

    virtual void Error(const char* format, ...) = 0;
    virtual void Warn(const char* format, ...) = 0;
    virtual void Info(const char* format, ...) = 0;
};