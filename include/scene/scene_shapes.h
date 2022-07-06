#ifndef SCENE_SHAPES_HPP
#define SCENE_SHAPES_HPP

#include <scene/transform.h>

namespace rt
{
    class SceneShape
    {
    public:
        Transform transform;

    private:
    public:
        SceneShape() {}
        virtual ~SceneShape() {}

        virtual std::ostream &toString(std::ostream &stream) const = 0;
    };

    std::ostream &operator<<(std::ostream &stream, const SceneShape &shape);

    namespace Shapes
    {
        class Sphere : public SceneShape
        {
            double r;

            virtual ~Sphere();

            virtual std::ostream &toString(std::ostream &stream) const override;
        };
    } // namespace Shapes

} // namespace rt

#endif // SCENE_SHAPES_HPP