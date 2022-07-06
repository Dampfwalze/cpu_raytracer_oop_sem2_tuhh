#include <scene/scene_shapes.h>

#include <stream_formatter.h>

namespace rt
{
    std::ostream &operator<<(std::ostream &stream, const SceneShape &shape)
    {
        shape.toString(stream);
        return stream;
    }

    namespace Shapes
    {
        Sphere::~Sphere() {}

        std::ostream &Sphere::toString(std::ostream &stream) const
        {
            stream << "Sphere { " << transform << ", radius: " << radius << "}";
            return stream;
        }
    }
}