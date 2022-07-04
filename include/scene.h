#ifndef __SCENE_H__
#define __SCENE_H__

#include <rtmath.h>

#include <vector>
#include <memory>
#include <optional>

namespace rt
{
    namespace m = math;

    class SceneShape
    {
    public:
        struct Transform
        {
            m::dvec3 position;
            m::dquat rotation;

        private:
            std::optional<m::dmat4> m_matrix;

        public:
            Transform();

            m::dmat4 const &getMatrix();
        } transform;

    private:
    public:
        SceneShape() {}
        virtual ~SceneShape() {}

        virtual std::ostream &toString(std::ostream &stream) const = 0;
    };

    std::ostream &operator<<(std::ostream &stream, const SceneShape &shape);
    std::ostream &operator<<(std::ostream &stream, const SceneShape::Transform &transform);

    namespace Shapes
    {
        class Sphere : public SceneShape
        {
            double r;

            virtual ~Sphere();

            virtual std::ostream &toString(std::ostream &stream) const override;
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

        friend std::ostream &operator<<(std::ostream &stream, const Scene &shape);
    };

    std::ostream &operator<<(std::ostream &stream, const Scene &shape);
} // namespace rt

#endif // __SCENE_H__