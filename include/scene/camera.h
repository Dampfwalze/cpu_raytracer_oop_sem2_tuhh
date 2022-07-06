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
        double zNear, zFar;

    public:
        Camera(const Transform &transform = Transform(), float FOV = 90, double zNear = 1, double zFar = 2);
        ~Camera();

        m::dmat4 getMatrix(double aspect) const;
    };

    std::ostream &operator<<(std::ostream &stream, const Camera &camera);
} // namespace rt

#endif // CAMERA_HPP