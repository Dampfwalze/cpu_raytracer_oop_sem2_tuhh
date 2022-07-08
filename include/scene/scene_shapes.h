#ifndef SCENE_SHAPES_HPP
#define SCENE_SHAPES_HPP

#include <scene/transform.h>

namespace rt
{
    class SceneShape
    {
    public:
        Transform transform;

    private:
    public:
        SceneShape(const Transform &transform = Transform())
            : transform(transform) {}
        virtual ~SceneShape() {}

        virtual std::ostream &toString(std::ostream &stream) const = 0;
    };

    std::ostream &operator<<(std::ostream &stream, const SceneShape &shape);

    namespace Shapes
    {
        struct Sphere : public SceneShape
        {
            double radius;

            Sphere(double radius = 1, const Transform &transform = Transform());
            virtual ~Sphere();

            virtual std::ostream &toString(std::ostream &stream) const override;
        };
    } // namespace Shapes

} // namespace rt

#endif // SCENE_SHAPES_HPP