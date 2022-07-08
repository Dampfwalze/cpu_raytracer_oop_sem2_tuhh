#include <scene/camera.h>
namespace rt
{
    Camera::Camera(const Transform &transform, float FOV, double zNear, double zFar)
        : transform(transform), FOV(FOV), zNear(zNear), zFar(zFar) {}

    Camera::~Camera()
    {
    }

    m::dmat4 Camera::getMatrix(double aspect) const
    {
        auto pers = m::perspective<double>(FOV, aspect, zNear, zFar);
        return pers * transform.getMatrix();
    }

    std::ostream &operator<<(std::ostream &stream, const Camera &camera)
    {
        stream << "Camera { FOV: " << camera.FOV << ", transform: " << camera.transform << " }";
        return stream;
    }
}