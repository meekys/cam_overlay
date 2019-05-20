#pragma once

#include <cerrno>
#include <string.h>

#include "Exception.hpp"

class ErrNoException : public Exception
{
public:
    ErrNoException():
        Exception(std::to_string(errno) + ", " + ::strerror(errno)),
        _errNo(errno)
    {}

    ErrNoException(std::string message):
        Exception(message + ": " + std::to_string(errno) + ", " + ::strerror(errno)),
        _errNo(errno)
    {}

private:
    int _errNo;
};