#ifndef SCENE_SHAPES_HPP
#define SCENE_SHAPES_HPP

#include <scene/transform.h>

#define CLASS_NAME(name) \
    constexpr const char *className() const override { return #name; }

namespace rt
{
    struct Intersection
    {
        m::dvec3 position;
        m::dvec3 normal;
    };

    class SceneShape
    {
    public:
        Transform transform;

    private:
    public:
        SceneShape(const Transform &transform = Transform())
            : transform(transform) {}
        virtual ~SceneShape() {}

        virtual std::optional<Intersection> intersect(const m::ray<double> &ray) const = 0;

        virtual std::ostream &toString(std::ostream &stream) const = 0;

        virtual constexpr const char *className() const = 0;
    };

    std::ostream &operator<<(std::ostream &stream, const SceneShape &shape);

    namespace Shapes
    {
        struct Sphere : public SceneShape
        {
            double radius;

            Sphere(double radius = 1, const Transform &transform = Transform());
            virtual ~Sphere();

            virtual std::optional<Intersection> intersect(const m::ray<double> &ray) const override;

            virtual std::ostream &toString(std::ostream &stream) const override;

            CLASS_NAME(Sphere)
        };
    } // namespace Shapes

} // namespace rt

#undef GET_NAME

#endif // SCENE_SHAPES_HPP