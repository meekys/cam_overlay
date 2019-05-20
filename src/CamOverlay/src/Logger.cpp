#include <cstdio>

#include <CamOverlay/Logger.hpp>

#define LOG(level, format) \
    va_list args;\
    va_start(args, format);\
    Log(level, format, args);\
    va_end(args);

void Logger::Error(const char* format, ...)
{
    LOG(0, format);
}

void Logger::Warn(const char* format, ...)
{
    LOG(0, format);
}

void Logger::Info(const char* format, ...)
{
    LOG(0, format);
}

void Logger::Log(int /*level*/, const char* format, va_list args)
{
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
}