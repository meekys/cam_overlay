#pragma once

#include <memory>
#include <vector>

#include <Core/ILogger.hpp>
#include <CamOverlay/IInput.hpp>
#include <CamOverlay/Config.hpp>

class InputFile : public IInput
{
public:
    InputFile(std::shared_ptr<ILogger> logger, std::shared_ptr<Config> config);

    virtual bool Read(InputData& data) override;
    virtual void EndRead() override {};

    virtual int getWidth() override { return _width; }
    virtual int getHeight() override { return _height; }

private:
    void ReadFile();

    std::shared_ptr<ILogger> _logger;
    std::shared_ptr<Config>  _config;

    int               _width;
    int               _height;
    std::string       _filename;
    std::vector<char> _buffer;
};