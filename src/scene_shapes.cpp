#include <scene/scene_shapes.h>

#include <stream_formatter.h>

#include <pixel_logger.h>
#include <rt_imgui.h>

namespace rt
{
    SceneShape::SceneShape(const std::string_view &name, const Transform &transform, size_t materialIndex)
        : transform(transform), materialIndex(materialIndex), SceneObject(name) {}

    bool SceneShape::onInspectorGUI()
    {
        return rtImGui::Drag("Transform", transform, 0.01f);
    }

    namespace Shapes
    {
        // Sphere

        Sphere::Sphere(const std::string_view &name, double radius, const Transform &transform, size_t materialIndex)
            : radius(radius), SceneShape(name, transform, materialIndex) {}
        Sphere::Sphere(double radius, const Transform &transform, size_t materialIndex)
            : radius(radius), SceneShape("Sphere", transform, materialIndex) {}

        std::optional<Intersection> Sphere::intersect(const m::ray<double> &ray) const
        {
            const m::dvec3 &o = ray.origin;
            const m::dvec3 &d = ray.direction;

            double a = m::dot(d, d);
            double b = 2 * (m::dot(o, d));
            double c = m::dot(o, o) - radius * radius;

            double result = b * b - 4 * a * c;

            if (result < 0)
                return std::nullopt;

            double t = (-b - std::sqrt(result)) / (2 * a);
            if (t < 0.01)
                return std::nullopt;

            Intersection i;
            i.position = ray(t);
            i.normal = i.position;
            i.object = (SceneShape *)this;
            i.sampleInfo = {
                .type = SampleInfoType::Direction,
                .asDirection = i.normal,
            };
            return i;
        }

        bool Sphere::onInspectorGUI()
        {
            return SceneShape::onInspectorGUI() | rtImGui::Drag("Radius", radius, 0.01f);
        }

        std::ostream &Sphere::toString(std::ostream &stream) const
        {
            stream << "Sphere { name: \"" << name << "\", transform: " << transform << ", radius: " << radius << "}";
            return stream;
        }

        // Plane

        Plane::Plane(const std::string_view &name, const Transform &transform, size_t materialIndex)
            : SceneShape(name, transform, materialIndex) {}
        Plane::Plane(const Transform &transform, size_t materialIndex)
            : SceneShape("Plane", transform, materialIndex) {}

        std::optional<Intersection> Plane::intersect(const m::ray<double> &ray) const
        {
            double t = -ray.origin.y / ray.direction.y;
            if (t < 0.01)
                return std::nullopt;
            Intersection i;
            i.position = ray(t);
            i.normal = m::dvec3(0, 1, 0);
            i.object = (SceneShape *)this;
            i.sampleInfo = {
                .type = SampleInfoType::UV,
                .asUV = i.position.xz(),
            };
            return i;
        }

        std::ostream &Plane::toString(std::ostream &stream) const
        {
            return stream << "Plane { name: \"" << name << "\", transform: " << transform << " }";
        }

        // Cube

        Cube::Cube(const std::string_view &name, const Transform &transform, size_t materialIndex)
            : SceneShape(name, transform, materialIndex) {}
        Cube::Cube(const Transform &transform, size_t materialIndex)
            : SceneShape("Cube", transform, materialIndex) {}

        template <size_t D, bool R = true>
        inline bool intersectCubeFace(Intersection *intersection, const m::ray<double> &ray)
        {
            const size_t D1 = (D + 1) % 3;
            const size_t D2 = (D + 2) % 3;

            double s = ray.direction[D] > 0 == R ? -1 : 1; // What face is nearest?
            double t = (-ray.origin[D] + s * 0.5) / ray.direction[D];
            auto   p = ray(t);

            if (t <= 0.01)
                return false;

            if (p[D1] <= 0.5 && p[D1] >= -0.5 && p[D2] <= 0.5 && p[D2] >= -0.5)
            {
                intersection->position = p;
                intersection->normal[D] = s;
                return true;
            }
            return false;
        }

        template <bool R = true>
        inline std::optional<Intersection> cubeIntersection(const m::ray<double> &ray)
        {
            Intersection intersection;

            if (intersectCubeFace<0, R>(&intersection, ray)) // For x
                return intersection;
            if (intersectCubeFace<1, R>(&intersection, ray)) // For y
                return intersection;
            if (intersectCubeFace<2, R>(&intersection, ray)) // For z
                return intersection;

            return std::nullopt;
        }

        std::optional<Intersection> Cube::intersect(const m::ray<double> &ray) const
        {
            auto intersection = cubeIntersection(ray);
            if (intersection)
                intersection->object = (SceneShape *)this;
            return intersection;
        }

        std::ostream &Cube::toString(std::ostream &stream) const
        {
            return stream << "Cube { name: \"" << name << "\", transform: " << transform << " }";
        }

        VoxelShape::VoxelShape(ResourceRef<Resources::VoxelGridResource> grid, const std::string_view &name, const Transform &transform, size_t materialIndex)
            : grid(grid), SceneShape(name, transform, materialIndex) {}
        VoxelShape::VoxelShape(ResourceRef<Resources::VoxelGridResource> grid, const Transform &transform, size_t materialIndex)
            : grid(grid), SceneShape("Voxel Grid", transform, materialIndex) {}

        template <size_t D>
        inline m::dvec3 calcU(const m::ray<double> &ray)
        {
            if (ray.direction[D] == 0)
                return {INFINITY, INFINITY, INFINITY};
            double h = (ray.direction[D] > 0) ? m::ceil(ray.origin[D] - 0.0001) : m::floor(ray.origin[D] + 0.0001);
            return ray.direction * ((h - ray.origin[D]) / ray.direction[D]);
        }

        std::optional<Intersection> VoxelShape::intersect(const m::ray<double> &ray_) const
        {
            if (!grid)
                return std::nullopt;
            VoxelGrid &grid = this->grid->grid;

            auto           size = (m::dvec3)grid.getSize();
            m::ray<double> ray;

            auto maybeIntersection = cubeIntersection(ray_);
            if (maybeIntersection)
                ray = m::ray<double>(maybeIntersection->position * size + size / 2.0,
                                     glm::normalize(ray_.direction));
            else
            {
                maybeIntersection = cubeIntersection<false>(ray_);
                if (maybeIntersection)
                    ray = m::ray<double>(ray_.origin * size + size / 2.0,
                                         glm::normalize(ray_.direction));
                else
                    return std::nullopt;
            }

            // Setup
            int g_x = (ray.direction.x < 0) ? 1 : 0;
            int g_y = (ray.direction.y < 0) ? 1 : 0;
            int g_z = (ray.direction.z < 0) ? 1 : 0;

            m::dvec3 u_x = calcU<0>(ray);
            m::dvec3 u_y = calcU<1>(ray);
            m::dvec3 u_z = calcU<2>(ray);

            m::dvec3 v_x = ray.direction * (ray.direction.x > 0 ? 1.0 : -1.0) / ray.direction.x;
            m::dvec3 v_y = ray.direction * (ray.direction.y > 0 ? 1.0 : -1.0) / ray.direction.y;
            m::dvec3 v_z = ray.direction * (ray.direction.z > 0 ? 1.0 : -1.0) / ray.direction.z;

            m::dvec3 r_x(ray.origin + u_x);
            m::dvec3 r_y(ray.origin + u_y);
            m::dvec3 r_z(ray.origin + u_z);

            // PIXEL_LOGGER_LOG(ray,
            //                  ", u_x: ", u_x,
            //                  ", u_y: ", u_y,
            //                  ", u_z: ", u_z,
            //                  ", v_x: ", v_x,
            //                  ", v_y: ", v_y,
            //                  ", v_z: ", v_z, ", ");

            while (true)
            {
                if (r_x.x < r_y.x != (bool)g_x && r_x.x < r_z.x != (bool)g_x)
                {
                    m::u64vec3 pos(m::round(r_x.x) - g_x, m::floor(r_x.y), m::floor(r_x.z));
                    // PIXEL_LOGGER_LOG("(", pos, ", ", r_x, "), ");
                    if (!(pos.x < size.x && pos.x >= 0 && pos.y < size.y && pos.y >= 0 && pos.z < size.z && pos.z >= 0))
                        break;
                    if (grid.at(pos).colorIndex)
                        return Intersection{
                            .position = (r_x - size / 2.0) / size,
                            .normal = m::dvec3(g_x ? 1 : -1, 0, 0),
                            .object = (SceneShape *)this,
                            .sampleInfo = {
                                .type = SampleInfoType::Index,
                                .asIndex = (unsigned)grid.at(pos).colorIndex,
                            },
                        };
                    r_x += v_x;
                }
                else if (r_y.y < r_z.y != (bool)g_y)
                {
                    m::u64vec3 pos(m::floor(r_y.x), m::round(r_y.y) - g_y, m::floor(r_y.z));
                    // PIXEL_LOGGER_LOG("(", pos, ", ", r_y, "), ");
                    if (!(pos.x < size.x && pos.x >= 0 && pos.y < size.y && pos.y >= 0 && pos.z < size.z && pos.z >= 0))
                        break;
                    if (grid.at(pos).colorIndex)
                        return Intersection{
                            .position = (r_y - size / 2.0) / size,
                            .normal = m::dvec3(0, g_y ? 1 : -1, 0),
                            .object = (SceneShape *)this,
                            .sampleInfo = {
                                .type = SampleInfoType::Index,
                                .asIndex = (unsigned)grid.at(pos).colorIndex,
                            },
                        };
                    r_y += v_y;
                }
                else
                {
                    m::u64vec3 pos(m::floor(r_z.x), m::floor(r_z.y), m::round(r_z.z) - g_z);
                    // PIXEL_LOGGER_LOG("(", pos, ", ", r_z, "), ");
                    if (!(pos.x < size.x && pos.x >= 0 && pos.y < size.y && pos.y >= 0 && pos.z < size.z && pos.z >= 0))
                        break;
                    if (grid.at(pos).colorIndex)
                        return Intersection{
                            .position = (r_z - size / 2.0) / size,
                            .normal = m::dvec3(0, 0, g_z ? 1 : -1),
                            .object = (SceneShape *)this,
                            .sampleInfo = {
                                .type = SampleInfoType::Index,
                                .asIndex = (unsigned)grid.at(pos).colorIndex,
                            },
                        };
                    r_z += v_z;
                }
            }

            // PIXEL_LOGGER_LOG("No voxel was hit!\n");

            return std::nullopt;
        }

        bool VoxelShape::onInspectorGUI()
        {
            bool changed = false;

            changed |= SceneShape::onInspectorGUI();
            changed |= rtImGui::ResourceBox("Voxel Grid", grid);

            return changed;
        }

        std::ostream &VoxelShape::toString(std::ostream &stream) const
        {
            return stream << "VoxelShape { name: \"" << name << "\", transform: " << transform << ", voxel grid: " << grid << " }";
        }
    }
}