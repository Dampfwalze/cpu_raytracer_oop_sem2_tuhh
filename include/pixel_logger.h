#ifndef PIXEL_LOGGER_HPP
#define PIXEL_LOGGER_HPP

#include <ostream>
#include <thread>

#define PIXEL_LOGGER_LOG(...)            \
    if (PixelLogger::logger.isLogging()) \
    PixelLogger::logger.log(__VA_ARGS__)

#ifdef _DEBUG
#ifdef __GNUC__
#define PIXEL_BREAK                      \
    if (PixelLogger::logger.isLogging()) \
    asm("int $3")
#elif defined(_MSC_VER)
#define PIXEL_BREAK                      \
    if (PixelLogger::logger.isLogging()) \
    __debugbreak()
#else
#define PIXEL_BREAK
#endif
#else
#define PIXEL_BREAK
#endif

namespace rt
{
    class PixelLogger
    {
    public:
        static PixelLogger logger;

    private:
        std::ostream *m_logger = nullptr;

        bool            m_logging = false;
        std::thread::id m_currentThreadId;

    public:
        PixelLogger();
        ~PixelLogger();

        void beginLog();
        void endLog();

        inline void setStream(std::ostream *stream) { m_logger = stream; }

        inline bool isLogging() const { return m_logging && m_currentThreadId == std::this_thread::get_id(); }

        template <typename T>
        PixelLogger &operator<<(const T &value)
        {
            if (isLogging())
                *m_logger << value;
            return *this;
        }

        template <typename T>
        void log(T val)
        {
            if (isLogging())
                *m_logger << val;
        }
        template <typename T, typename... Args>
        void log(T val, Args... vals)
        {
            if (isLogging())
            {
                *m_logger << val;
                log(vals...);
            }
        }
    };
} // namespace rt

#endif // PIXEL_LOGGER_HPP