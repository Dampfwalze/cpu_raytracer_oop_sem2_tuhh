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

        struct Cached
        {
            m::dmat4 matrix;
            m::dmat4 inverseMatrix;
        } mutable cached;

    public:
        Camera(const Transform &transform = Transform(), float FOV = glm::radians(80.0), double zNear = 0.01, double zFar = 2);
        ~Camera();

        void cacheMatrix(double aspect) const;
    };

    std::ostream &operator<<(std::ostream &stream, const Camera &camera);
} // namespace rt

#endif // CAMERA_HPP