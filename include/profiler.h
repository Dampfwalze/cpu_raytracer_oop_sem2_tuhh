#ifndef PROFILER_HPP
#define PROFILER_HPP

#include <map>
#include <vector>
#include <thread>
#include <memory>
#include <chrono>
#include <mutex>

namespace Profiling
{
    using duration = std::chrono::microseconds;
    using time_point = std::chrono::time_point<std::chrono::high_resolution_clock, duration>;

    std::ostream &operator<<(std::ostream &stream, const duration &duration);
    std::ostream &operator<<(std::ostream &stream, const time_point &timePoint);

    struct Task
    {
        const char *Label;

        time_point start;
        time_point end;

        Task(const char *label, time_point start, time_point end = time_point());

        inline duration getTime() const { return end - start; }
    };

    std::ostream &operator<<(std::ostream &stream, const Task &task);

    struct FrameProfile
    {
        std::map<std::thread::id, std::vector<Task>> tasks;
        time_point start;
        time_point end;
    };

    std::ostream &operator<<(std::ostream &stream, const FrameProfile &profile);

    class Profiler
    {
        std::unique_ptr<FrameProfile> currentProfile;
        std::mutex profileMutex;

    public:
        bool enabled = true;

        inline FrameProfile *getProfile() { return currentProfile.get(); }
        std::unique_ptr<FrameProfile> exchangeProfile();

        void beginFrame();
        void endFrame();

        void profileTask(const char *Label);
        void endTask();
    };

    extern Profiler profiler;
} // namespace Profiling

namespace rtImGui
{
    void drawProfiler(const char *label, const Profiling::FrameProfile &profile, float height = 200);
} // namespace rtImGui

#endif // PROFILER_HPP