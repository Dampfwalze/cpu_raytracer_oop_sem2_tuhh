#include <pixel_logger.h>

namespace rt
{
    PixelLogger PixelLogger::logger = PixelLogger();

    PixelLogger::PixelLogger() {}
    PixelLogger::~PixelLogger() {}

    void PixelLogger::beginLog()
    {
        if (m_logger == nullptr)
            return;
        m_logging = true;
        m_currentThreadId = std::this_thread::get_id();
    }

    void PixelLogger::endLog()
    {
        if (m_currentThreadId == std::this_thread::get_id())
            m_logging = false;
    }
} // namespace rt
