#ifndef SCENE_HPP
#define SCENE_HPP

#include <scene/camera.h>
#include <scene/material.h>
#include <scene/scene_lights.h>
#include <scene/scene_shapes.h>

#include <map>
#include <memory>
#include <vector>

namespace rt
{
    class Scene
    {
    public:
        using shape_collection_type = std::vector<std::unique_ptr<SceneShape>>;
        using light_collection_type = std::vector<std::unique_ptr<SceneLight>>;
        using material_collection_type = std::map<size_t, std::unique_ptr<Material>>;

    public:
        shape_collection_type objects;

        light_collection_type lights;

        material_collection_type materials;

        Camera camera;

        SamplerRef<> environmentTexture;

    public:
        Scene(shape_collection_type &objects, const Camera &camera = Camera());
        Scene(shape_collection_type &&objects = shape_collection_type(), const Camera &camera = Camera());
        Scene(const Camera &camera);
        ~Scene();

        void   addShape(SceneShape *shape);
        void   addLight(SceneLight *light);
        size_t addMaterial(Material *material);

        Material *getMaterial(size_t index) const;

        void cacheFrameData(const m::u64vec2 &screenSize) const;

        std::optional<Intersection> castRay(const m::ray<double> &ray, std::optional<double> maxLength2 = std::nullopt) const;

        bool onInspectorGUI();

        friend std::ostream &operator<<(std::ostream &stream, const Scene &shape);
    };

    std::ostream &operator<<(std::ostream &stream, const Scene &scene);
} // namespace rt

#endif // SCENE_HPP