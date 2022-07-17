#include <iostream>
#include <exception>

#include <application.h>
#include <window.h>
#include <stdlib.h>
#include <filesystem>

int main(int argc, char const *argv[])
{
    std::cout << "Hello Ray Tracing!" << std::endl;
    std::filesystem::path originPath(std::filesystem::path(argv[0]).parent_path());

    try
    {
        rt::Application application(originPath);
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
