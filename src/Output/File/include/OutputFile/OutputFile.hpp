#pragma once

#include <memory>
#include <fstream>

#include <Core/ILogger.hpp>
#include <CamOverlay/IOutput.hpp>
#include <CamOverlay/Config.hpp>
#include <CamOverlay/IInput.hpp>

class OutputFile : public IOutput
{
public:
    static bool SupportsFormat(std::string format);

    OutputFile(std::shared_ptr<ILogger> logger, std::shared_ptr<Config> config, std::shared_ptr<IInput> input);

    virtual void Update(void* data, int size) override;
    virtual void Draw() override;

private:
    std::shared_ptr<ILogger>   _logger;
    std::shared_ptr<Config>    _config;
    std::shared_ptr<IInput>    _input;

    std::string   _baseName;
    std::ofstream _file;
};