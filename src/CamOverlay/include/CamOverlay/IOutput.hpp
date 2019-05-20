#pragma once

class IOutput
{
public:
    virtual ~IOutput() = default;
    
    virtual void Update(void* data, int size) = 0;
    virtual void Draw() = 0;

private:
};