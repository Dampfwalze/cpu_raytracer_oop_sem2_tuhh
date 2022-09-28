#ifndef SCENE_DESERIALIZER_HPP
#define SCENE_DESERIALIZER_HPP

#include <yaml-cpp/yaml.h>

#include <resource_container.h>
#include <rtmath.h>
#include <scene/scene.h>

#include <filesystem>
#include <fstream>

namespace rt
{
    namespace m = rt::math;

    template <typename T, typename Type>
    concept PtrOfType = std::is_same<Type, typename T::element_type>::value ||(std::is_pointer<T>::value &&std::is_same<Type, std::remove_pointer_t<T>>::value);

    class SceneDeserializer
    {
    private:
        ResourceContainer *m_resources;

        std::filesystem::path m_originPath;

    public:
        SceneDeserializer(ResourceContainer *resources, const std::filesystem::path &originPath)
            : m_resources(resources), m_originPath(originPath) {}
        ~SceneDeserializer() = default;

        Scene *deserializeStream(std::istream &stream);
        Scene *deserializeFile(const std::filesystem::path &path);
        Scene *deserializeString(const std::string &string);
        Scene *deserializeNode(const YAML::Node &node);

    private:
        using ResourceMap = std::map<std::string, ResourceRef<void>>;
        ResourceMap resources;
        using MaterialMap = std::map<std::string, size_t>;
        MaterialMap materials;

        ResourceMap &deserialize(ResourceMap &resources, const YAML::Node &node);
        template <typename T>
        ResourceRef<T> &deserialize(ResourceRef<T> &ref, const YAML::Node &node);

        Scene &deserialize(Scene &scene, const YAML::Node &node);

        Scene::shape_collection_type    &deserialize(Scene::shape_collection_type &shapes, const YAML::Node &node);
        Scene::light_collection_type    &deserialize(Scene::light_collection_type &lights, const YAML::Node &node);
        Scene::material_collection_type &deserialize(Scene::material_collection_type &materials, const YAML::Node &node);
        Camera                          &deserialize(Camera &camera, const YAML::Node &node);

        template <typename T>
        requires PtrOfType<T, SceneShape>
            T              &deserialize(T &ptr, const YAML::Node &node);
        Shapes::Plane      &deserialize(Shapes::Plane &plane, const YAML::Node &node);
        Shapes::Sphere     &deserialize(Shapes::Sphere &sphere, const YAML::Node &node);
        Shapes::Cube       &deserialize(Shapes::Cube &cube, const YAML::Node &node);
        Shapes::VoxelShape &deserialize(Shapes::VoxelShape &voxel, const YAML::Node &node);

        template <typename T>
        requires PtrOfType<T, Material>
            T                  &deserialize(T &ptr, const YAML::Node &node);
        Materials::LitMaterial &deserialize(Materials::LitMaterial &material, const YAML::Node &node);

        size_t &deserializeMaterial(size_t &index, const YAML::Node &node);

        template <typename T>
        requires PtrOfType<T, SceneLight>
            T                    &deserialize(T &ptr, const YAML::Node &node);
        Lights::PointLight       &deserialize(Lights::PointLight &light, const YAML::Node &node);
        Lights::DirectionalLight &deserialize(Lights::DirectionalLight &light, const YAML::Node &node);

        SamplerRef<Sampler>      &deserialize(SamplerRef<Sampler> &ptr, const YAML::Node &node);
        Samplers::ColorSampler   &deserialize(Samplers::ColorSampler &sampler, const YAML::Node &node);
        Samplers::TextureSampler &deserialize(Samplers::TextureSampler &sampler, const YAML::Node &node);
        Samplers::PaletteSampler &deserialize(Samplers::PaletteSampler &sampler, const YAML::Node &node);

        Samplers::TextureSampler::WrapMethod   &deserialize(Samplers::TextureSampler::WrapMethod &method, const YAML::Node &node);
        Samplers::TextureSampler::FilterMethod &deserialize(Samplers::TextureSampler::FilterMethod &method, const YAML::Node &node);

        Transform       &deserialize(Transform &transform, const YAML::Node &node);
        m::dvec3        &deserialize(m::dvec3 &vec, const YAML::Node &node);
        m::dquat        &deserialize(m::dquat &quat, const YAML::Node &node);
        m::Color<float> &deserialize(m::Color<float> &color, const YAML::Node &node);
    };
} // namespace rt

#endif // SCENE_DESERIALIZER_HPP
