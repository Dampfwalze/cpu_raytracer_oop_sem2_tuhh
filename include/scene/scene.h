#ifndef SCENE_HPP
#define SCENE_HPP

#include <scene/scene_shapes.h>

#include <vector>

namespace rt
{
    class Scene
    {
    public:
        using shape_collection_type = std::vector<std::unique_ptr<SceneShape>>;

    private:
        shape_collection_type m_objects;

    public:
        Scene(shape_collection_type &objects);
        Scene(shape_collection_type &&objects);
        Scene();
        ~Scene();

        void addShape(SceneShape *shape);

        friend std::ostream &operator<<(std::ostream &stream, const Scene &shape);
    };

    std::ostream &operator<<(std::ostream &stream, const Scene &scene);
} // namespace rt

#endif // SCENE_HPP