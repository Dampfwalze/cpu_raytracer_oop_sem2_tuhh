#include <application.h>

namespace rt
{
    Application::Application()
    {
    }

    Application::~Application()
    {
    }

    void Application::run()
    {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
}