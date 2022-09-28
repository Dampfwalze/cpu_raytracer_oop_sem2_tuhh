#ifndef SCENE_SERIALIZER_HPP
#define SCENE_SERIALIZER_HPP

#include <rtmath.h>
#include <scene/scene.h>

#include <yaml-cpp/yaml.h>

#include <iostream>

namespace rt
{
    namespace m = math;

    struct SceneSerializerData
    {
        std::map<ResourceRef<Resource>, std::string> resourceMap;
    };

    class SceneSerializer
    {
    private:
    public:
        SceneSerializer() = default;
        ~SceneSerializer() = default;

        void        serialize(const Scene &scene, const std::string_view &path);
        std::string serialize(const Scene &scene);
        void        serialize(const Scene &scene, std::ostream &stream);
        void        serialize(const Scene &scene, YAML::Emitter &emitter);

    private:
        SceneSerializerData m_data;
    };

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Camera &camera);

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const SceneShape &shape);
    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Shapes::Plane &plane);
    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Shapes::Sphere &sphere);
    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Shapes::Cube &cube);
    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Shapes::VoxelShape &voxel);

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const SceneLight &light);
    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Lights::PointLight &light);
    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Lights::DirectionalLight &light);
    // YAML::Emitter &operator<<(YAML::Emitter &emitter, const Lights::SpotLight &light);

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Material &material);
    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Materials::LitMaterial &material);

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const SamplerRef<Sampler> &sampler);
    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Sampler &sampler);
    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Samplers::ColorSampler &sampler);
    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Samplers::TextureSampler &sampler);
    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Samplers::PaletteSampler &sampler);

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Samplers::TextureSampler::FilterMethod &method);
    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Samplers::TextureSampler::WrapMethod &method);

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Transform &transform);
    YAML::Emitter &operator<<(YAML::Emitter &emitter, const m::dvec3 &vec);
    YAML::Emitter &operator<<(YAML::Emitter &emitter, const m::dquat &quat);
    YAML::Emitter &operator<<(YAML::Emitter &emitter, const m::Color<float> &color);

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const ResourceRef<Resource> &resource);
    template <typename T>
    YAML::Emitter &operator<<(YAML::Emitter &emitter, const ResourceRef<T> &resource)
    {
        return emitter << *(ResourceRef<Resource> *)(&resource);
    }

    template <typename T>
    concept is_dereferencable = std::is_pointer<T>::value || std::is_same_v<decltype(std::declval<T>().operator*()), std::add_lvalue_reference_t<typename std::pointer_traits<T>::element_type>>;

    template <typename T>
    requires is_dereferencable<T>
        YAML::Emitter &operator<<(YAML::Emitter &emitter, const T &item)
    {
        return emitter << *item;
    }

} // namespace rt

#endif // SCENE_SERIALIZER_HPP