#include <imgui.h>
#include <rtmath.h>
#include <scene/transform.h>

namespace rtImGui
{
    namespace m = rt::math;

    template <typename T>
    constexpr ImGuiDataType imGuiTypeFrom();

    template <>
    constexpr ImGuiDataType imGuiTypeFrom<char>() { return ImGuiDataType_S8; }
    template <>
    constexpr ImGuiDataType imGuiTypeFrom<signed char>() { return ImGuiDataType_S8; }
    template <>
    constexpr ImGuiDataType imGuiTypeFrom<unsigned char>() { return ImGuiDataType_U8; }
    template <>
    constexpr ImGuiDataType imGuiTypeFrom<short>() { return ImGuiDataType_S16; }
    template <>
    constexpr ImGuiDataType imGuiTypeFrom<unsigned short>() { return ImGuiDataType_U16; }
    template <>
    constexpr ImGuiDataType imGuiTypeFrom<int>() { return ImGuiDataType_S32; }
    template <>
    constexpr ImGuiDataType imGuiTypeFrom<unsigned int>() { return ImGuiDataType_U32; }
    template <>
    constexpr ImGuiDataType imGuiTypeFrom<long long>() { return ImGuiDataType_S64; }
    template <>
    constexpr ImGuiDataType imGuiTypeFrom<unsigned long long>() { return ImGuiDataType_U64; }
    template <>
    constexpr ImGuiDataType imGuiTypeFrom<float>() { return ImGuiDataType_Float; }
    template <>
    constexpr ImGuiDataType imGuiTypeFrom<double>() { return ImGuiDataType_Double; }

    template <typename T>
    struct _drag_detail
    {
        static bool Drag(const char *label, T &value, float v_speed,
                         const void *p_min,
                         const void *p_max,
                         const char *format,
                         ImGuiSliderFlags flags)
        {
            return ImGui::DragScalar(label, imGuiTypeFrom<T>(), &value, v_speed, p_min, p_max, format, flags);
        }
    };

    template <int D, typename T>
    struct _drag_detail<m::vec<D, T>>
    {
        static bool Drag(const char *label, m::vec<D, T> &value, float v_speed,
                         const void *p_min,
                         const void *p_max,
                         const char *format,
                         ImGuiSliderFlags flags)
        {
            return ImGui::DragScalarN(label, imGuiTypeFrom<T>(), &value, D, v_speed, p_min, p_max, format, flags);
        }
    };

    template <typename T>
    struct _drag_detail<m::qua<T>>
    {
        static bool Drag(const char *label, m::qua<T> &value, float v_speed,
                         const void *p_min,
                         const void *p_max,
                         const char *format,
                         ImGuiSliderFlags flags)
        {
            return ImGui::DragScalarN(label, imGuiTypeFrom<T>(), &value, 4, v_speed, p_min, p_max, format, flags);
        }
    };

    template <typename T, typename F = std::nullptr_t>
    inline bool Drag(const char *label, T &value, float v_speed,
                     const std::optional<F> &p_min = std::nullopt,
                     const std::optional<F> &p_max = std::nullopt,
                     const char *format = nullptr,
                     ImGuiSliderFlags flags = 0)
    {
        F _p_min = p_min.value_or(F());
        F _p_max = p_max.value_or(F());

        return _drag_detail<T>::Drag(label, value, v_speed,
                                     p_min.has_value() ? &_p_min : nullptr,
                                     p_max.has_value() ? &_p_max : nullptr,
                                     format, flags);
    }

    template <>
    inline bool Drag<rt::Transform, double>(const char *label, rt::Transform &value, float v_speed,
                                            const std::optional<double> &p_min,
                                            const std::optional<double> &p_max,
                                            const char *format,
                                            ImGuiSliderFlags flags)
    {
        if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool res = Drag("Position", value.position, v_speed, p_min, p_max, format, flags) |
                       Drag("Rotation", value.rotation, v_speed, p_min, p_max, format, flags);
            ImGui::TreePop();
            return res;
        }
        return false;
    }

    template <>
    inline bool Drag<rt::Transform, std::nullptr_t>(const char *label, rt::Transform &value, float v_speed,
                                                    const std::optional<std::nullptr_t> &p_min,
                                                    const std::optional<std::nullptr_t> &p_max,
                                                    const char *format,
                                                    ImGuiSliderFlags flags)
    {
        return Drag<rt::Transform, double>(label, value, v_speed);
    }
} // namespace rtImGui
