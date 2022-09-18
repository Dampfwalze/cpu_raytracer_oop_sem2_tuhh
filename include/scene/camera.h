#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <scene/scene_object.h>
#include <scene/transform.h>

namespace rt
{
    class Camera : public SceneObject
    {
    public:
        Transform transform;
        float     FOV;
        double    zNear, zFar;

        struct Cached
        {
            m::dmat4 matrix;
            m::dmat4 inverseMatrix;
        } mutable cached;

    public:
        Camera(const std::string_view &name, const Transform &transform = Transform(), float FOV = glm::radians(80.0), double zNear = 0.01, double zFar = 2);
        Camera(const Transform &transform = Transform(), float FOV = glm::radians(80.0), double zNear = 0.01, double zFar = 2);

        virtual std::ostream &toString(std::ostream &stream) const override;

        virtual bool onInspectorGUI() override;

        void cacheMatrix(double aspect) const;
    };
} // namespace rt

#endif // CAMERA_HPP