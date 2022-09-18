#include <rt_imgui.h>
#include <scene/camera.h>

namespace rt
{
    Camera::Camera(const std::string_view &name, const Transform &transform, float FOV, double zNear, double zFar)
        : transform(transform), FOV(FOV), zNear(zNear), zFar(zFar), SceneObject(name) {}
    Camera::Camera(const Transform &transform, float FOV, double zNear, double zFar)
        : transform(transform), FOV(FOV), zNear(zNear), zFar(zFar), SceneObject("Camera") {}

    std::ostream &Camera::toString(std::ostream &stream) const
    {
        return stream << "Camera { name: \"" << name << "\", FOV: " << FOV << ", transform: " << transform << " }";
    }

    bool Camera::onInspectorGUI()
    {
        bool changed = false;
        changed |= rtImGui::Drag<Transform, double>("Transform", transform, 0.01f, std::nullopt, std::nullopt, "%.03f");
        int a = (int)m::degrees(FOV);
        changed |= rtImGui::Drag<int, int>("FOV", a, 1, 30, 120);
        FOV = m::radians((float)a);
        changed |= rtImGui::Drag<double, double>("zNear", zNear, 1.0f, 0.00001, 1000, "%f", ImGuiSliderFlags_Logarithmic);
        changed |= rtImGui::Drag<double, double>("zFar", zFar, 1.0f, 0.0001, 1000, "%f", ImGuiSliderFlags_Logarithmic);
        return changed;
    }

    void Camera::cacheMatrix(double aspect) const
    {
        transform.cacheMatrix();
        auto pers = m::perspective<double>(FOV, aspect, zNear, zFar);
        cached.matrix = pers * transform.cached.inverseMatrix;
        cached.inverseMatrix = m::inverse(cached.matrix);
    }
}