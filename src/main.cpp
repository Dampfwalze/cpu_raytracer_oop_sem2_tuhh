#include <exception>
#include <iostream>

#include <application.h>
#include <filesystem>
#include <stdlib.h>
#include <tclap/CmdLine.h>
#include <window.h>

struct Args
{
    bool                       useGui;
    std::optional<std::string> sceneFile;
    std::optional<std::string> output;
    rt::m::u64vec2             size;

    static Args parse(int argc, const char *const *argv)
    {
        TCLAP::CmdLine cmd("Ray Tracing", ' ', "0.1");

        TCLAP::SwitchArg             guiArg("g", "nogui", "Don't use GUI", cmd, true);
        TCLAP::ValueArg<std::string> sceneArg("s", "scene", "Scene file", false, "", "string", cmd);
        TCLAP::ValueArg<int64_t>     widthArg("", "width", "Width of the output", false, 1920, "int", cmd);
        TCLAP::ValueArg<int64_t>     heightArg("", "height", "Height of the output", false, 1080, "int", cmd);
        TCLAP::ValueArg<std::string> outputArg("o", "output", "Output file", false, "", "string", cmd);

        cmd.parse(argc, argv);

        return Args{
            .useGui = guiArg.getValue(),
            .sceneFile = sceneArg.isSet() ? std::optional(sceneArg.getValue()) : std::nullopt,
            .output = outputArg.isSet() ? std::optional(outputArg.getValue()) : std::nullopt,
            .size = {widthArg.getValue(), heightArg.getValue()},
        };
    }
};

int main(int argc, char const *argv[])
{
    std::filesystem::path originPath(std::filesystem::path(argv[0]).parent_path());

    Args args = Args::parse(argc, argv);

    try
    {
        rt::Application application(originPath, args.useGui, args.sceneFile, args.output, args.size);
        application.run();
    }
    catch (const std::exception &e)
    {
        std::cout << "Application failed:\n"
                  << e.what() << std::endl;
        return -1;
    }
    catch (const std::error_code &code)
    {
        std::cout << "Application failed with code " << code.value()
                  << ":\nCategory: " << code.category().name()
                  << "\nMessage: " << code.message()
                  << std::endl;
        return code.value();
    }
    catch (...)
    {
        std::cout << "Application failed:\nError description unavailable" << std::endl;
        return -1;
    }
    return 0;
}
