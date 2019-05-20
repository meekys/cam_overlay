#pragma once

#include "InputData.hpp"

class IInput
{
public:
    virtual ~IInput() = default;

    virtual bool Read(InputData& data) = 0;
    virtual void EndRead() = 0;

    virtual int getWidth() = 0;
    virtual int getHeight() = 0;

private:
};