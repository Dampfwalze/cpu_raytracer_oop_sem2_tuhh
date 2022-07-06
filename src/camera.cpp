#include <scene/camera.h>
namespace rt
{
    Camera::Camera(const Transform &transform, float FOV)
        : transform(transform), FOV(FOV) {}

    Camera::~Camera()
    {
    }

    std::ostream &operator<<(std::ostream &stream, const Camera &camera)
    {
        stream << "Camera { FOV: " << camera.FOV << ", transform: " << camera.transform << " }";
        return stream;
    }
}