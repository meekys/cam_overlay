#pragma once

#include <exception>
#include <string>

using namespace std::string_literals;

class Exception : public std::exception
{
public:
    Exception(std::string message)
    {
        _message = message;
    }

    virtual const char* what() const noexcept override
    {
        return _message.c_str();
    }

private:
    std::string _message;
};