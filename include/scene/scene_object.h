#ifndef SCENE_OBJECT_HPP
#define SCENE_OBJECT_HPP

#include <string>

namespace rt
{
    class SceneObject
    {
    public:
        std::string name;

        SceneObject(const std::string_view &name);
        virtual ~SceneObject() = default;

        virtual bool onInspectorGUI();

        virtual std::ostream &toString(std::ostream &stream) const = 0;
    };

    std::ostream &operator<<(std::ostream &stream, const SceneObject &object);
} // namespace rt

#endif // SCENE_OBJECT_HPP