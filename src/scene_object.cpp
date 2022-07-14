#include <scene/scene_object.h>

namespace rt
{
    SceneObject::SceneObject(const std::string_view &name)
        : name(name) {}

    bool SceneObject::onInspectorGUI() { return false; }

    std::ostream &operator<<(std::ostream &stream, const SceneObject &object)
    {
        object.toString(stream);
        return stream;
    }
}