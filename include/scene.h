#ifndef __SCENE_H__
#define __SCENE_H__

#include <vector>
#include <memory>

namespace rt
{
    class SceneObject
    {
    private:
    public:
        SceneObject() {}
        ~SceneObject() {}
    };

    class Scene
    {
    private:
        std::vector<std::unique_ptr<SceneObject>> m_objects;

    public:
        Scene(std::vector<std::unique_ptr<SceneObject>> &objects);
        Scene(std::vector<std::unique_ptr<SceneObject>> &&objects);
        Scene();
        ~Scene();
    };

} // namespace rt

#endif // __SCENE_H__