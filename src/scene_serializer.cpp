#include <scene/scene_serializer.h>

#include <fstream>
#include <sstream>
#include <typeinfo>

namespace rt
{
    thread_local SceneSerializerData *_data;

    void SceneSerializer::serialize(const Scene &scene, const std::filesystem::path &path)
    {
        std::ofstream file(path);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file for writing");
        }
        return serialize(scene, file);
    }

    std::string SceneSerializer::serialize(const Scene &scene)
    {
        std::stringstream stream;
        serialize(scene, stream);
        return stream.str();
    }

    void SceneSerializer::serialize(const Scene &scene, std::ostream &stream)
    {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        serialize(scene, emitter);
        emitter << YAML::EndMap;
        stream << emitter.c_str();
    }

    void SceneSerializer::serialize(const Scene &scene, YAML::Emitter &emitter)
    {
        _data = &m_data;
        emitter << YAML::Key << "scene" << YAML::Value;

        emitter << YAML::BeginMap
                << YAML::Key << "camera" << YAML::Value << scene.camera
                << YAML::Key << "shapes" << YAML::Value << scene.objects
                << YAML::Key << "lights" << YAML::Value << scene.lights
                << YAML::Key << "materials" << YAML::Value << scene.materials
                << YAML::Key << "environment" << YAML::Value << scene.environmentTexture
                << YAML::EndMap;

        emitter << YAML::Key << "resources" << YAML::Value;

        emitter << YAML::BeginMap;
        for (auto &&[resource, id] : _data->resourceMap)
        {
            emitter << YAML::Key << id << YAML::Value << resource.getPath().string();
        }
        emitter << YAML::EndMap;
        _data = nullptr;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const SceneShape &shape)
    {
        auto &type = typeid(shape);
        if (type == typeid(Shapes::Plane))
        {
            emitter << YAML::LocalTag("plane") << (Shapes::Plane &)shape;
        }
        else if (type == typeid(Shapes::Sphere))
        {
            emitter << YAML::LocalTag("sphere") << (Shapes::Sphere &)shape;
        }
        else if (type == typeid(Shapes::Cube))
        {
            emitter << YAML::LocalTag("cube") << (Shapes::Cube &)shape;
        }
        else if (type == typeid(Shapes::VoxelShape))
        {
            emitter << YAML::LocalTag("voxel") << (Shapes::VoxelShape &)shape;
        }
        else
        {
            throw std::runtime_error("Unknown shape type: " + std::string(type.name()));
        }
        return emitter;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Shapes::Plane &plane)
    {
        return emitter << YAML::BeginMap
                       << YAML::Key << "name" << YAML::Value << plane.name
                       << YAML::Key << "transform" << YAML::Value << plane.transform
                       << YAML::Key << "material" << YAML::Value << plane.materialIndex
                       << YAML::EndMap;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Shapes::Sphere &sphere)
    {
        return emitter << YAML::BeginMap
                       << YAML::Key << "name" << YAML::Value << sphere.name
                       << YAML::Key << "transform" << YAML::Value << sphere.transform
                       << YAML::Key << "radius" << YAML::Value << sphere.radius
                       << YAML::Key << "material" << YAML::Value << sphere.materialIndex
                       << YAML::EndMap;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Shapes::Cube &cube)
    {
        return emitter << YAML::BeginMap
                       << YAML::Key << "name" << YAML::Value << cube.name
                       << YAML::Key << "transform" << YAML::Value << cube.transform
                       << YAML::Key << "material" << YAML::Value << cube.materialIndex
                       << YAML::EndMap;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Shapes::VoxelShape &voxel)
    {
        return emitter << YAML::BeginMap
                       << YAML::Key << "name" << YAML::Value << voxel.name
                       << YAML::Key << "transform" << YAML::Value << voxel.transform
                       << YAML::Key << "material" << YAML::Value << voxel.materialIndex
                       << YAML::Key << "voxel_grid" << YAML::Value << voxel.grid
                       << YAML::EndMap;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const SceneLight &light)
    {
        auto &type = typeid(light);
        if (type == typeid(Lights::PointLight))
        {
            emitter << YAML::LocalTag("point") << (Lights::PointLight &)light;
        }
        else if (type == typeid(Lights::DirectionalLight))
        {
            emitter << YAML::LocalTag("directional") << (Lights::DirectionalLight &)light;
        }
        // else if (type == typeid(Lights::SpotLight))
        // {
        //     emitter << YAML::LocalTag("spot") << (Lights::SpotLight &)light;
        // }
        else
        {
            throw std::runtime_error("Unknown light type: " + std::string(type.name()));
        }
        return emitter;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Lights::PointLight &light)
    {
        return emitter << YAML::BeginMap
                       << YAML::Key << "position" << YAML::Value << light.position
                       << YAML::Key << "intensity" << YAML::Value << light.intensity
                       << YAML::Key << "color" << YAML::Value << light.color
                       << YAML::EndMap;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Lights::DirectionalLight &light)
    {
        return emitter << YAML::BeginMap
                       << YAML::Key << "direction" << YAML::Value << light.direction
                       << YAML::Key << "intensity" << YAML::Value << light.intensity
                       << YAML::Key << "color" << YAML::Value << light.color
                       << YAML::EndMap;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Material &material)
    {
        auto &type = typeid(material);
        if (type == typeid(Materials::LitMaterial))
        {
            emitter << YAML::LocalTag("lit") << (Materials::LitMaterial &)material;
        }
        else
        {
            throw std::runtime_error("Unknown material type: " + std::string(type.name()));
        }
        return emitter;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Materials::LitMaterial &material)
    {
        return emitter << YAML::BeginMap
                       << YAML::Key << "name" << YAML::Value << material.name
                       << YAML::Key << "color" << YAML::Value << material.color
                       << YAML::Key << "ambient" << YAML::Value << material.ambient
                       << YAML::Key << "diffuse" << YAML::Value << material.diffuse
                       << YAML::Key << "specular" << YAML::Value << material.specular
                       << YAML::Key << "reflection" << YAML::Value << material.reflection
                       << YAML::EndMap;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const SamplerRef<Sampler> &sampler)
    {
        return emitter << *sampler;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Sampler &sampler)
    {
        auto &type = typeid(sampler);
        if (type == typeid(Samplers::ColorSampler))
        {
            emitter << YAML::LocalTag("color") << (Samplers::ColorSampler &)sampler;
        }
        else if (type == typeid(Samplers::TextureSampler))
        {
            emitter << YAML::LocalTag("texture") << (Samplers::TextureSampler &)sampler;
        }
        else if (type == typeid(Samplers::PaletteSampler))
        {
            emitter << YAML::LocalTag("palette") << (Samplers::PaletteSampler &)sampler;
        }
        else
        {
            throw std::runtime_error("Unknown sampler type: " + std::string(type.name()));
        }
        return emitter;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Samplers::ColorSampler &sampler)
    {
        return emitter << YAML::BeginMap
                       << YAML::Key << "name" << YAML::Value << sampler.name
                       << YAML::Key << "color" << YAML::Value << sampler.color
                       << YAML::EndMap;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Samplers::TextureSampler &sampler)
    {
        return emitter << YAML::BeginMap
                       << YAML::Key << "name" << YAML::Value << sampler.name
                       << YAML::Key << "texture" << YAML::Value << sampler.texture
                       << YAML::Key << "filter" << YAML::Value << sampler.filterMethod
                       << YAML::Key << "wrap" << YAML::Value << sampler.wrapMethod
                       << YAML::EndMap;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Samplers::PaletteSampler &sampler)
    {
        return emitter << YAML::BeginMap
                       << YAML::Key << "name" << YAML::Value << sampler.name
                       << YAML::Key << "palette" << YAML::Value << sampler.palette
                       << YAML::EndMap;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Samplers::TextureSampler::FilterMethod &method)
    {
        static const std::map<Samplers::TextureSampler::FilterMethod, std::string> methodNames = {
            {Samplers::TextureSampler::FilterMethod::Nearest, "nearest"},
            {Samplers::TextureSampler::FilterMethod::Linear, "linear"},
        };
        return emitter << methodNames.at(method);
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Samplers::TextureSampler::WrapMethod &method)
    {
        static const std::map<Samplers::TextureSampler::WrapMethod, std::string> methodNames = {
            {Samplers::TextureSampler::WrapMethod::Clamp, "clamp"},
            {Samplers::TextureSampler::WrapMethod::Repeat, "repeat"},
        };
        return emitter << methodNames.at(method);
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Camera &camera)
    {
        return emitter << YAML::BeginMap
                       << YAML::Key << "transform" << YAML::Value << camera.transform
                       << YAML::Key << "fov" << YAML::Value << glm::degrees(camera.FOV)
                       << YAML::Key << "near" << YAML::Value << camera.zNear
                       << YAML::Key << "far" << YAML::Value << camera.zFar
                       << YAML::EndMap;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const Transform &transform)
    {
        return emitter << YAML::BeginMap
                       << YAML::Key << "position" << YAML::Value << transform.position
                       << YAML::Key << "rotation" << YAML::Value << transform.rotation
                       << YAML::Key << "scale" << YAML::Value << transform.scale
                       << YAML::EndMap;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const m::dvec3 &vec)
    {
        return emitter << YAML::Flow << YAML::BeginSeq
                       << vec.x << vec.y << vec.z
                       << YAML::EndSeq;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const m::dquat &quat)
    {
        return emitter << YAML::Flow << YAML::BeginSeq
                       << quat.x << quat.y << quat.z << quat.w
                       << YAML::EndSeq;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const m::Color<float> &color)
    {
        uint32_t bin = static_cast<uint32_t>(color.r * 255.0f) << 16 |
                       static_cast<uint32_t>(color.g * 255.0f) << 8 |
                       static_cast<uint32_t>(color.b * 255.0f);
        return emitter << YAML::Hex << bin;
    }

    YAML::Emitter &operator<<(YAML::Emitter &emitter, const ResourceRef<Resource> &resource)
    {
        if (_data == nullptr)
        {
            return emitter << resource.getPath().string();
        }

        std::string_view id;

        auto &resMap = _data->resourceMap;

        auto it = resMap.find(resource);
        if (it == resMap.end())
            id = resMap[resource] = "resource_" + std::to_string(resMap.size());
        else
            id = it->second;

        return emitter << std::string(id);
    }

} // namespace rt
