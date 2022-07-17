#ifndef SCENE_SHAPES_HPP
#define SCENE_SHAPES_HPP

#include <scene/transform.h>
#include <scene/scene_object.h>
#include <voxel_grid.h>
#include <resources.h>

#include <memory>

namespace rt
{
    class SceneShape;

    struct Intersection
    {
        m::dvec3 position;
        m::dvec3 normal;
        SceneShape *object;
    };

    class SceneShape : public SceneObject
    {
    public:
        Transform transform;
        size_t materialIndex;

    private:
    public:
        SceneShape(const std::string_view &name, const Transform &transform = Transform(), size_t materialIndex = 0);
        virtual ~SceneShape() = default;

        virtual std::optional<Intersection> intersect(const m::ray<double> &ray) const = 0;

        virtual bool onInspectorGUI() override;
    };

    namespace Shapes
    {
        struct Sphere : public SceneShape
        {
            double radius;

            Sphere(const std::string_view &name, double radius = 1, const Transform &transform = Transform(), size_t materialIndex = 0);
            Sphere(double radius = 1, const Transform &transform = Transform(), size_t materialIndex = 0);

            virtual std::optional<Intersection> intersect(const m::ray<double> &ray) const override;

            virtual bool onInspectorGUI() override;

            virtual std::ostream &toString(std::ostream &stream) const override;
        };

        struct Plane : public SceneShape
        {
            Plane(const std::string_view &name, const Transform &transform = Transform(), size_t materialIndex = 0);
            Plane(const Transform &transform = Transform(), size_t materialIndex = 0);

            virtual std::optional<Intersection> intersect(const m::ray<double> &ray) const override;

            virtual std::ostream &toString(std::ostream &stream) const override;
        };

        struct Cube : public SceneShape
        {
            Cube(const std::string_view &name, const Transform &transform = Transform(), size_t materialIndex = 0);
            Cube(const Transform &transform = Transform(), size_t materialIndex = 0);

            virtual std::optional<Intersection> intersect(const m::ray<double> &ray) const override;

            virtual std::ostream &toString(std::ostream &stream) const override;
        };

        struct VoxelShape : public SceneShape
        {
            ResourceRef<Resources::VoxelGridResource> grid;

            VoxelShape(ResourceRef<Resources::VoxelGridResource> grid, const std::string_view &name, const Transform &transform = Transform(), size_t materialIndex = 0);
            VoxelShape(ResourceRef<Resources::VoxelGridResource> grid, const Transform &transform = Transform(), size_t materialIndex = 0);

            virtual std::optional<Intersection> intersect(const m::ray<double> &ray) const override;

            virtual std::ostream &toString(std::ostream &stream) const override;
        };
    } // namespace Shapes

} // namespace rt

#endif // SCENE_SHAPES_HPP