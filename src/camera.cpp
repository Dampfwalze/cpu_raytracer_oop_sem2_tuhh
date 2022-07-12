#include <scene/camera.h>
namespace rt
{
    Camera::Camera(const Transform &transform, float FOV, double zNear, double zFar)
        : transform(transform), FOV(FOV), zNear(zNear), zFar(zFar) {}

    Camera::~Camera()
    {
    }

    void Camera::cacheMatrix(double aspect) const
    {
        transform.cacheMatrix();
        auto pers = m::perspective<double>(FOV, aspect, zNear, zFar);
        cached.matrix = pers * transform.cached.matrix;
        cached.inverseMatrix = m::inverse(cached.matrix);
    }

    std::ostream &operator<<(std::ostream &stream, const Camera &camera)
    {
        stream << "Camera { FOV: " << camera.FOV << ", transform: " << camera.transform << " }";
        return stream;
    }
}