#ifndef __SCENE_H__
#define __SCENE_H__

#include <vector>
#include <memory>

namespace rt
{
    class SceneShape
    {
    private:
    public:
        SceneShape() {}
        ~SceneShape() {}
    };

    namespace Shapes
    {
        class Sphere : public SceneShape
        {
            double r;
        };
    }

    class Scene
    {
    public:
        using shape_collection_type = std::vector<std::unique_ptr<SceneShape>>;

    private:
        shape_collection_type m_objects;

    public:
        Scene(std::vector<std::unique_ptr<SceneShape>> &objects);
        Scene(std::vector<std::unique_ptr<SceneShape>> &&objects);
        Scene();
        ~Scene();

        void addShape(SceneShape *shape);
    };

} // namespace rt

#endif // __SCENE_H__