#ifndef SCENE_HPP
#define SCENE_HPP

#include <scene/scene_shapes.h>
#include <scene/camera.h>
#include <material.h>

#include <vector>
#include <map>
#include <memory>

namespace rt
{
    class Scene
    {
    public:
        using shape_collection_type = std::vector<std::unique_ptr<SceneShape>>;
        using material_collection_type = std::map<size_t, std::unique_ptr<Material>>;

    public:
        shape_collection_type objects;

        material_collection_type materials;

        Camera camera;

    public:
        Scene(shape_collection_type &objects, const Camera &camera = Camera());
        Scene(shape_collection_type &&objects = shape_collection_type(), const Camera &camera = Camera());
        Scene(const Camera &camera);
        ~Scene();

        void addShape(SceneShape *shape);
        size_t addMaterial(Material *material);

        Material *getMaterial(size_t index) const;

        void cacheFrameData(const m::u64vec2 &screenSize) const;

        std::optional<Intersection> castRay(const m::ray<double> &ray) const;

        friend std::ostream &operator<<(std::ostream &stream, const Scene &shape);
    };

    std::ostream &operator<<(std::ostream &stream, const Scene &scene);
} // namespace rt

#endif // SCENE_HPP