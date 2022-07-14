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

    void Camera::cacheMatrix(double aspect) const
    {
        transform.cacheMatrix();
        auto pers = m::perspective<double>(FOV, aspect, zNear, zFar);
        cached.matrix = pers * transform.cached.matrix;
        cached.inverseMatrix = m::inverse(cached.matrix);
    }
}