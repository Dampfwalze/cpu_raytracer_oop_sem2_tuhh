#ifndef SCENE_HPP
#define SCENE_HPP

#include <scene/scene_shapes.h>
#include <scene/camera.h>

#include <vector>

namespace rt
{
    class Scene
    {
    public:
        using shape_collection_type = std::vector<std::unique_ptr<SceneShape>>;

    public:
        shape_collection_type objects;

        Camera camera;

    public:
        Scene(shape_collection_type &objects, const Camera &camera = Camera());
        Scene(shape_collection_type &&objects = shape_collection_type(), const Camera &camera = Camera());
        Scene(const Camera &camera);
        ~Scene();

        void addShape(SceneShape *shape);

        friend std::ostream &operator<<(std::ostream &stream, const Scene &shape);
    };

    std::ostream &operator<<(std::ostream &stream, const Scene &scene);
} // namespace rt

#endif // SCENE_HPP