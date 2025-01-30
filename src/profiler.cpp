#include <profiler.h>

#include <rtmath.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace Profiling
{
    std::ostream &operator<<(std::ostream &stream, const duration &duration)
    {
        return stream << duration.count() / 1000 << "ms";
    }

    std::ostream &operator<<(std::ostream &stream, const time_point &timePoint)
    {
        return stream << timePoint.time_since_epoch();
    }

    Task::Task(const char *label, time_point start, time_point end)
        : Label(label), start(start), end(end) {}

    std::ostream &operator<<(std::ostream &stream, const Task &task)
    {
        return stream << task.Label << ": " << task.getTime().count() / 1000.0 << "ms"
                      << "\tstart: " << task.start << "\tend: " << task.end;
    }

    std::ostream &operator<<(std::ostream &stream, const FrameProfile &profile)
    {
        stream << "Start: " << profile.start << "\nEnd: " << profile.end << "\nDuration: " << profile.end - profile.start << "\n";
        for (auto &&thread : profile.tasks)
        {
            stream << "Thread " << thread.first << ":\n";
            for (auto &&task : thread.second)
            {
                stream << " -- " << task << "\n";
            }
        }
        return stream;
    }

    std::unique_ptr<FrameProfile> Profiler::exchangeProfile()
    {
        std::lock_guard<std::mutex> lk(profileMutex);
        return std::move(currentProfile);
    }

    void Profiler::beginFrame()
    {
        if (!enabled)
            return;

        std::lock_guard<std::mutex> lk(profileMutex);
        currentProfile = std::make_unique<FrameProfile>();
        currentProfile->start = std::chrono::time_point_cast<duration>(std::chrono::high_resolution_clock::now());
    }

    void Profiler::endFrame()
    {
        if (!enabled)
            return;

        auto time = std::chrono::time_point_cast<duration>(std::chrono::high_resolution_clock::now());

        std::lock_guard<std::mutex> lk(profileMutex);

        if (currentProfile == nullptr)
            return;

        currentProfile->end = time;
        for (auto &&thread : currentProfile->tasks)
        {
            if (!thread.second.empty())
                thread.second.back().end = time;
        }
    }

    void Profiler::profileTask(const char *Label)
    {
        if (!enabled)
            return;

        auto time = std::chrono::time_point_cast<duration>(std::chrono::high_resolution_clock::now());

        std::lock_guard<std::mutex> lk(profileMutex);

        if (currentProfile == nullptr)
            return;

        auto &threadTasks = currentProfile->tasks[std::this_thread::get_id()];

        if (!threadTasks.empty() && threadTasks.back().end.time_since_epoch() == time_point::duration::zero())
            threadTasks.back().end = time;

        threadTasks.emplace_back(Label, time);
    }

    void Profiler::endTask()
    {
        if (!enabled)
            return;

        auto time = std::chrono::time_point_cast<duration>(std::chrono::high_resolution_clock::now());

        std::lock_guard<std::mutex> lk(profileMutex);

        if (currentProfile == nullptr)
            return;

        auto &threadTasks = currentProfile->tasks[std::this_thread::get_id()];
        if (!threadTasks.empty())
            threadTasks.back().end = time;
    }

    Profiler profiler;
}

namespace rtImGui
{
    namespace m = rt::math;

    static void drawTaskRect(const ImRect &rect, ImColor col, const char *label)
    {
        auto drawList = ImGui::GetWindowDrawList();

        auto darkened = col;

        const float darkenFact = 0.8f;
        darkened.Value.x *= darkenFact;
        darkened.Value.y *= darkenFact;
        darkened.Value.z *= darkenFact;

        drawList->AddRectFilled(rect.Min, rect.Max, col, 2);
        drawList->AddRect(rect.Min, rect.Max, darkened, 2);

        if (rect.GetWidth() < 10)
            return;

        auto textSize = ImGui::CalcTextSize(label);

        auto textPos = rect.GetCenter() - textSize / 2;
        textPos.x = glm::max(rect.Min.x, textPos.x);

        drawList->PushClipRect(rect.Min, rect.Max, true);
        drawList->AddText(textPos, 0xffffffff, label);
        drawList->PopClipRect();
    }

    void drawProfiler(const char *label, const Profiling::FrameProfile &profile, float height)
    {
        ImGuiWindow *window = ImGui::GetCurrentWindow();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::BeginChild(label, ImVec2(window->ContentRegionRect.GetWidth(), height), true, ImGuiWindowFlags_HorizontalScrollbar);
        {
            ImGui::PopStyleVar();

            ImGuiWindow  *window = ImGui::GetCurrentWindow();
            ImDrawList   *drawList = window->DrawList;
            ImGuiStorage *storage = window->DC.StateStorage;
            ImGuiIO       IO = ImGui::GetIO();

            ImGuiID zoom_id = window->GetID("Zoom");
            ImGuiID pressed_id = window->GetID("Pressed");

            float zoom = storage->GetFloat(zoom_id, 1);
            float scroll = window->Scroll.x;

            auto viewport = ImRect(window->ContentRegionRect.Min + window->Scroll, window->ContentRegionRect.Max + window->Scroll);
            auto size = ImVec2(viewport.GetWidth() * zoom, viewport.GetHeight());

            // Handle zoom
            float wheel = IO.MouseWheel;
            if (wheel && ImGui::IsWindowHovered())
            {
                float mouse = IO.MousePos.x - viewport.Min.x;

                float start = -scroll;
                float end = (size.x - scroll);

                float _start = start - mouse;
                float _end = end - mouse;

                _start *= 1 + wheel * 0.2f;
                _end *= 1 + wheel * 0.2f;

                start = glm::min(_start + mouse, 0.0f);
                end = glm::max(_end + mouse, viewport.GetWidth());

                scroll = -start;
                zoom = (end + scroll) / viewport.GetWidth();

                ImGui::SetScrollX(scroll);
                storage->SetFloat(zoom_id, zoom);
                size.x = viewport.GetWidth() * zoom;

                // float mouse = m::map(IO.MousePos.x, viewport.Min.x, viewport.Max.x, 0.0f, 1.0f);
                //
                // float start = -scroll / viewport.GetWidth();
                // float end = (size.x - scroll) / viewport.GetWidth();
                //
                // float _start = start - mouse;
                // float _end = end - mouse;
                //
                //_start *= 1 + wheel * 0.2;
                //_end *= 1 + wheel * 0.2;
                //
                // start = glm::min(_start + mouse, 0.0f);
                // end = glm::max(_end + mouse, 1.0f);
                //
                // scroll = -viewport.GetWidth() * start;
                // zoom = (end + scroll / viewport.GetWidth());
                //
                // ImGui::SetScrollX(scroll);
                // storage->SetFloat(zoom_id, zoom);
                // size.x = viewport.GetWidth() * zoom;
            }

            // Handle dragging
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false) && ImGui::IsWindowHovered())
                storage->SetBool(pressed_id, true);
            else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                storage->SetBool(pressed_id, false);

            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && storage->GetBool(pressed_id, false))
            {
                float delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).x;
                ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);

                scroll = glm::max(glm::min(scroll - delta, window->ScrollMax.x), 0.0f);
                ImGui::SetScrollX(scroll);
            }

            ImGui::InvisibleButton("Label", size);
            ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

            float startX = viewport.Min.x - scroll;
            float endX = startX + size.x;

            // ImGui::GetForegroundDrawList()->AddRect(ImVec2(startX, viewport.Min.y), ImVec2(endX, viewport.Max.y), IM_COL32(255, 0, 0, 255));

            // Draw profile
            static const ImColor colors[] = {
                0xff1726fc,
                0xffab1c00,
                0xff288f00,
                0xff008f28,
                0xff8a0029,
                0xff56008f,
                0xff00778f,
                0xff5d8f00,
                0xff995065,
            };
            size_t currentColor = 0;

            std::map<const char *, size_t> colorMap;

            float height = viewport.GetHeight() / profile.tasks.size();
            auto  startTime = profile.start;
            auto  profileDuration = (profile.end - profile.start).count();
            int   i = 0;
            for (auto &&thread : profile.tasks)
            {
                for (size_t j = 0; j < thread.second.size(); j++)
                {
                    const Profiling::Task &task = thread.second[j];

#pragma warning(push)
#pragma warning(disable : 4244)
                    ImVec2 p1(m::map<double>((task.start - startTime).count(), 0, profileDuration, startX, endX), i * height + viewport.Min.y);
                    ImVec2 p2(m::map<double>((task.end - startTime).count(), 0, profileDuration, startX, endX), (i + 1) * height + viewport.Min.y);
#pragma warning(pop)

                    auto col = colorMap.find(task.Label);
                    if (col == colorMap.end())
                    {
                        colorMap[task.Label] = currentColor;
                        currentColor = (currentColor + 1) % (sizeof(colors) / sizeof(colors[0]));
                        col = colorMap.find(task.Label);
                    }

                    std::stringstream ss;
                    ss << task.getTime().count() / 1000.0 << "ms " << task.Label;
                    if (p2.x - p1.x > 0 && p2.x > viewport.Min.x && p1.x < viewport.Max.x)
                        drawTaskRect(ImRect(p1, p2), colors[col->second], ss.str().c_str());
                }

                i++;
            }
        }
        ImGui::EndChild();
    }
}