#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <window_thread.h>

namespace rt
{
    class Application
    {
    private:
        WindowThread window;

    public:
        Application();
        ~Application();

        void run();
    };

} // namespace rt

#endif // APPLICATION_HPP