#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <scene/transform.h>

namespace rt
{
    class Camera
    {
    public:
        Transform transform;
        float FOV;

    public:
        Camera(const Transform &transform = Transform(), float FOV = 90);
        ~Camera();
    };

    std::ostream &operator<<(std::ostream &stream, const Camera &camera);
} // namespace rt

#endif // CAMERA_HPP