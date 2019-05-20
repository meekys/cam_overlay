#include <di.hpp>

#include "App.hpp"
#include <CamOverlay/Config.hpp>
#include <CamOverlay/Logger.hpp>

#include "Generated/DisplayModules.hpp"
#include "Generated/InputModules.hpp"
#include "Generated/OutputModules.hpp"

namespace di = boost::di;

template <class T>
std::shared_ptr<IDisplay> CheckDisplay(const auto& injector, std::shared_ptr<Config> config, std::string module)
{
    if (config->display == module)
        return injector.template create<std::shared_ptr<T>>();

    return nullptr;
}

template <class T>
std::shared_ptr<IInput> CheckInput(const auto& injector, std::shared_ptr<Config> config, std::string module)
{
    if (config->input == module)
        return injector.template create<std::shared_ptr<T>>();

    return nullptr;
}

template <class T>
std::shared_ptr<IOutput> CheckOutput(const auto& injector, std::shared_ptr<Config> config, std::string module)
{
    if (config->output == module)
    {
        if (T::SupportsFormat(config->OutputFormat))
            return injector.template create<std::shared_ptr<T>>();

        throw Exception("Output type '" + module + "' does not support output format of '" + config->OutputFormat + "'");
    }   
    
    if (config->output == "" && T::SupportsFormat(config->OutputFormat))
        return injector.template create<std::shared_ptr<T>>();

    return nullptr;
}

int main(int argc, char *argv[])
{
    Args args { argc, argv };

    auto config_injector = di::make_injector(
        di::bind<Args>().to(args),
        di::bind<ILogger>().to<Logger>()
    );
    auto config = config_injector.create<std::shared_ptr<Config>>();

    auto core_module = [&config]
    {
        return di::make_injector(
            di::bind<Config>().to(config),
            di::bind<ILogger>().to<Logger>()
        );
    };

    auto display_module = [&config]
    {
        return di::make_injector(
            di::bind<IDisplay>().to([&](const auto& injector) -> std::shared_ptr<IDisplay> {
                std::shared_ptr<IDisplay> result;

                DISPLAY_MODULES

                throw Exception("Invalid display type: "s + config->display);
            })
        );
    };

    auto input_module = [&config]
    {
        return di::make_injector(
            di::bind<IInput>().to([&](const auto& injector) -> std::shared_ptr<IInput> {
                std::shared_ptr<IInput> result;

                INPUT_MODULES

                throw Exception("Invalid input type: "s + config->input);
            })
        );
    };

    auto output_module = [&config]
    {
        return di::make_injector(
            di::bind<IOutput>().to([&](const auto& injector) -> std::shared_ptr<IOutput> {
                std::shared_ptr<IOutput> result;

                OUTPUT_MODULES

                throw Exception("No output type: "s + config->output + " for format '" + config->OutputFormat + "'");
            })
        );
    };

    const auto injector = di::make_injector(
        core_module(),
        display_module(),
        input_module(),
        output_module()
    );

    auto app = injector.create<App>();
    app.Run();
}