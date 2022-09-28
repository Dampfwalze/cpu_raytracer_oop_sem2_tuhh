#include <fstream>
#include <scene/scene_deserializer.h>
#include <sstream>

namespace rt
{
    Scene *SceneDeserializer::deserializeStream(std::istream &stream)
    {
        YAML::Node doc = YAML::Load(stream);
        return deserializeNode(doc);
    }

    Scene *SceneDeserializer::deserializeFile(const std::filesystem::path &path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            auto p = m_originPath / path;
            file.open(p);
            if (!file.is_open())
                throw std::runtime_error("Could not open file: " + p.string());
        }

        return deserializeStream(file);
    }

    Scene *SceneDeserializer::deserializeString(const std::string &string)
    {
        YAML::Node doc = YAML::Load(string);
        return deserializeNode(doc);
    }

#define assertNode(exp, err) \
    if (!(exp))              \
    throw std::runtime_error(err)

    Scene *SceneDeserializer::deserializeNode(const YAML::Node &node)
    {
        assertNode(node.IsMap(), "Root node must be a map");

        resources.clear();
        deserialize(resources, node["resources"]);

        auto scene = std::make_unique<Scene>();
        deserialize((Scene &)*scene, node["scene"]);
        return scene.release();
    }

    SceneDeserializer::ResourceMap &SceneDeserializer::deserialize(ResourceMap &resources, const YAML::Node &node)
    {
        if (node)
        {
            assertNode(node.IsMap(), "Resources node must be a map");
            for (const auto &resource : node)
            {
                auto name = resource.first.as<std::string>();
                auto path = resource.second.as<std::string>();
                resources.emplace(name, *m_resources += path);
            }
        }
        return resources;
    }

    template <typename T>
    ResourceRef<T> &SceneDeserializer::deserialize(ResourceRef<T> &ref, const YAML::Node &node)
    {
        assertNode(node.IsScalar(), "Resource reference must be a scalar");
        ref = resources.at(node.as<std::string>());
        return ref;
    }

    size_t &SceneDeserializer::deserializeMaterial(size_t &index, const YAML::Node &node)
    {
        if (node)
        {
            assertNode(node.IsScalar(), "Material must be a scalar");
            index = materials.at(node.as<std::string>());
        }
        return index;
    }

    YAML::Node _getAlias(const YAML::Node &node, const char *arg)
    {
        return node[arg];
    }
    template <typename... Args>
    YAML::Node _getAlias(const YAML::Node &node, const char *arg, Args... args)
    {
        auto n = node[arg];
        if (n)
            return n;
        return _getAlias(node, args...);
    }

    Scene &SceneDeserializer::deserialize(Scene &scene, const YAML::Node &node)
    {
        assertNode(node, "Scene node not found");
        assertNode(node.IsMap(), "Scene node must be a map");

        deserialize(scene.materials, _getAlias(node, "materials", "material", "mat", "mats", "m"));
        deserialize(scene.objects, _getAlias(node, "shapes", "shape", "objects", "object", "objs", "obj", "s"));
        deserialize(scene.lights, _getAlias(node, "lights", "light", "l"));
        deserialize(scene.camera, _getAlias(node, "camera", "cam", "c"));
        deserialize(scene.environmentTexture, _getAlias(node, "environment_tex", "environment", "env_tex", "env", "e"));

        return scene;
    }

    Scene::shape_collection_type &SceneDeserializer::deserialize(Scene::shape_collection_type &shapes, const YAML::Node &node)
    {
        for (const auto &shape : node)
        {
            deserialize(shapes.emplace_back(nullptr), shape);
        }
        return shapes;
    }

    Scene::light_collection_type &SceneDeserializer::deserialize(Scene::light_collection_type &lights, const YAML::Node &node)
    {
        for (const auto &light : node)
        {
            deserialize(lights.emplace_back(nullptr), light);
        }
        return lights;
    }

    Scene::material_collection_type &SceneDeserializer::deserialize(Scene::material_collection_type &materials, const YAML::Node &node)
    {
        if (node)
        {
            assertNode(node.IsMap(), "Materials node must be a map");
            materials.clear();
            for (const auto &material : node)
            {
                auto id = this->materials.size();
                this->materials.emplace(material.first.as<std::string>(), id);
                deserialize(materials[id], material.second);
            }
        }
        return materials;
    }

    Camera &SceneDeserializer::deserialize(Camera &camera, const YAML::Node &node)
    {
        if (node)
        {
            assertNode(node.IsMap(), "Camera node must be a map");

            auto nameNode = node["name"];
            camera.name = nameNode ? nameNode.as<std::string>() : "Camera";

            deserialize(camera.transform, node["transform"]);

            if (auto fovNode = node["fov"])
                camera.FOV = fovNode.as<float>();

            if (auto nearNode = node["near"])
                camera.zNear = nearNode.as<double>();

            if (auto farNode = node["far"])
                camera.zFar = farNode.as<double>();
        }
        return camera;
    }

    template <typename T>
    requires PtrOfType<T, SceneShape>
        T &SceneDeserializer::deserialize(T &ptr, const YAML::Node &node)
    {
        auto tag = node.Tag();
        if (tag == "!sphere")
        {
            ptr = T(new Shapes::Sphere());
            deserialize((Shapes::Sphere &)*ptr, node);
        }
        else if (tag == "!plane")
        {
            ptr = T(new Shapes::Plane());
            deserialize((Shapes::Plane &)*ptr, node);
        }
        else if (tag == "!cube")
        {
            ptr = T(new Shapes::Cube());
            deserialize((Shapes::Cube &)*ptr, node);
        }
        else if (tag == "!voxel")
        {
            ptr = T(new Shapes::VoxelShape());
            deserialize((Shapes::VoxelShape &)*ptr, node);
        }
        else
        {
            assertNode(false, "Unknown shape type: " + tag);
        }
        return ptr;
    }

    Shapes::Sphere &SceneDeserializer::deserialize(Shapes::Sphere &sphere, const YAML::Node &node)
    {
        if (node && node.size() != 0)
        {
            assertNode(node.IsMap(), "Sphere node must be a map");

            auto nameNode = node["name"];
            sphere.name = nameNode ? nameNode.as<std::string>() : "Sphere";

            deserialize(sphere.transform, node["transform"]);

            deserializeMaterial(sphere.materialIndex, node["material"]);
        }
        return sphere;
    }

    Shapes::Plane &SceneDeserializer::deserialize(Shapes::Plane &plane, const YAML::Node &node)
    {
        if (node)
        {
            assertNode(node.IsMap(), "Plane node must be a map");

            auto nameNode = node["name"];
            plane.name = nameNode ? nameNode.as<std::string>() : "Plane";

            deserialize(plane.transform, node["transform"]);

            deserializeMaterial(plane.materialIndex, node["material"]);
        }
        return plane;
    }

    Shapes::Cube &SceneDeserializer::deserialize(Shapes::Cube &cube, const YAML::Node &node)
    {
        if (node)
        {
            assertNode(node.IsMap(), "Cube node must be a map");

            auto nameNode = node["name"];
            cube.name = nameNode ? nameNode.as<std::string>() : "Cube";

            deserialize(cube.transform, node["transform"]);

            deserializeMaterial(cube.materialIndex, node["material"]);
        }
        return cube;
    }

    Shapes::VoxelShape &SceneDeserializer::deserialize(Shapes::VoxelShape &voxel, const YAML::Node &node)
    {
        if (node)
        {
            assertNode(node.IsMap(), "Voxel node must be a map");

            auto nameNode = node["name"];
            voxel.name = nameNode ? nameNode.as<std::string>() : "Voxel";

            deserialize(voxel.transform, node["transform"]);

            deserializeMaterial(voxel.materialIndex, node["material"]);

            if (auto voxelNode = node["voxel_grid"])
                deserialize(voxel.grid, voxelNode);
        }
        return voxel;
    }

    template <typename T>
    requires PtrOfType<T, Material>
        T &SceneDeserializer::deserialize(T &ptr, const YAML::Node &node)
    {
        auto tag = node.Tag();
        if (tag == "!lit")
        {
            ptr = T(new Materials::LitMaterial(nullptr));
            deserialize((Materials::LitMaterial &)*ptr, node);
        }
        else
        {
            assertNode(false, "Unknown material type: " + tag);
        }
        return ptr;
    }

    Materials::LitMaterial &SceneDeserializer::deserialize(Materials::LitMaterial &material, const YAML::Node &node)
    {
        if (node && node.size() != 0)
        {
            assertNode(node.IsMap(), "Lit material node must be a map");

            auto nameNode = node["name"];
            material.name = nameNode ? nameNode.as<std::string>() : "Lit Material";

            deserialize(material.color, node["color"]);

            if (auto ambientNode = node["ambient"])
                material.ambient = ambientNode.as<float>();
            if (auto diffuseNode = node["diffuse"])
                material.diffuse = diffuseNode.as<float>();
            if (auto specularNode = node["specular"])
                material.specular = specularNode.as<float>();
            if (auto reflectionNode = node["reflection"])
                material.reflection = reflectionNode.as<float>();
        }
        return material;
    }

    template <typename T>
    requires PtrOfType<T, SceneLight>
        T &SceneDeserializer::deserialize(T &ptr, const YAML::Node &node)
    {
        auto tag = node.Tag();
        if (tag == "!directional")
        {
            ptr = T(new Lights::DirectionalLight());
            deserialize((Lights::DirectionalLight &)*ptr, node);
        }
        else if (tag == "!point")
        {
            ptr = T(new Lights::PointLight());
            deserialize((Lights::PointLight &)*ptr, node);
        }
        // else if (tag == "!spot")
        // {
        //     ptr = T(new Lights::SpotLight());
        //     deserialize((Lights::SpotLight &)*ptr, node);
        // }
        else
        {
            assertNode(false, "Unknown light type: " + tag);
        }
        return ptr;
    }

    Lights::DirectionalLight &SceneDeserializer::deserialize(Lights::DirectionalLight &light, const YAML::Node &node)
    {
        if (node && node.size() != 0)
        {
            assertNode(node.IsMap(), "Directional light node must be a map");

            auto nameNode = node["name"];
            light.name = nameNode ? nameNode.as<std::string>() : "Directional Light";

            deserialize(light.direction, node["direction"]);

            deserialize(light.color, node["color"]);

            if (auto intensityNode = node["intensity"])
                light.intensity = intensityNode.as<float>();
        }
        return light;
    }

    Lights::PointLight &SceneDeserializer::deserialize(Lights::PointLight &light, const YAML::Node &node)
    {
        if (node && node.size() != 0)
        {
            assertNode(node.IsMap(), "Point light node must be a map");

            auto nameNode = node["name"];
            light.name = nameNode ? nameNode.as<std::string>() : "Point Light";

            deserialize(light.position, node["position"]);

            deserialize(light.color, node["color"]);

            if (auto intensityNode = node["intensity"])
                light.intensity = intensityNode.as<float>();
        }
        return light;
    }

    SamplerRef<Sampler> &SceneDeserializer::deserialize(SamplerRef<Sampler> &ptr, const YAML::Node &node)
    {
        if (node)
        {
            auto tag = node.Tag();
            if (tag == "!color")
            {
                ptr = SamplerRef<Sampler>(new Samplers::ColorSampler());
                deserialize((Samplers::ColorSampler &)*ptr, node);
            }
            else if (tag == "!texture")
            {
                ptr = SamplerRef<Sampler>(new Samplers::TextureSampler());
                deserialize((Samplers::TextureSampler &)*ptr, node);
            }
            else if (tag == "!palette")
            {
                ptr = SamplerRef<Sampler>(new Samplers::PaletteSampler());
                deserialize((Samplers::PaletteSampler &)*ptr, node);
            }
            else
            {
                assertNode(false, "Unknown sampler type: " + tag);
            }
        }
        return ptr;
    }

    Samplers::ColorSampler &SceneDeserializer::deserialize(Samplers::ColorSampler &sampler, const YAML::Node &node)
    {
        if (node)
        {
            if (node.IsSequence() || node.IsScalar())
            { // parameter is inlined
                deserialize(sampler.color, node);
            }
            else
            {
                assertNode(node.IsMap(), "Color sampler node must be a map");

                auto nameNode = node["name"];
                sampler.name = nameNode ? nameNode.as<std::string>() : "Color Sampler";

                if (node["r"])
                { // parameter is inlined
                    deserialize(sampler.color, node);
                }
                else
                {
                    deserialize(sampler.color, node["color"]);
                }
            }
        }
        return sampler;
    }

    Samplers::TextureSampler &SceneDeserializer::deserialize(Samplers::TextureSampler &sampler, const YAML::Node &node)
    {
        if (node)
        {
            if (node.IsScalar())
            { // parameter is inlined
                deserialize(sampler.texture, node);
            }
            else
            {
                assertNode(node.IsMap(), "Texture sampler node must be a map");

                auto nameNode = node["name"];
                sampler.name = nameNode ? nameNode.as<std::string>() : "Texture Sampler";

                auto textureNode = node["texture"];
                assertNode(textureNode, "Texture sampler must have a texture");
                deserialize(sampler.texture, textureNode);

                deserialize(sampler.filterMethod, node["filterMethod"]);
                deserialize(sampler.wrapMethod, node["wrapMethod"]);
            }
        }
        return sampler;
    }

    Samplers::PaletteSampler &SceneDeserializer::deserialize(Samplers::PaletteSampler &sampler, const YAML::Node &node)
    {
        if (node)
        {
            if (node.IsScalar())
            { // parameter is inlined
                deserialize(sampler.palette, node);
            }
            else
            {
                assertNode(node.IsMap(), "Palette sampler node must be a map");

                auto nameNode = node["name"];
                sampler.name = nameNode ? nameNode.as<std::string>() : "Palette Sampler";

                auto paletteNode = node["palette"];
                assertNode(paletteNode, "Palette sampler must have a palette");
                deserialize(sampler.palette, paletteNode);
            }
        }
        return sampler;
    }

    Samplers::TextureSampler::WrapMethod &SceneDeserializer::deserialize(Samplers::TextureSampler::WrapMethod &method, const YAML::Node &node)
    {
        if (node)
        {
            static std::map<std::string, Samplers::TextureSampler::WrapMethod> methods = {
                {"clamp", Samplers::TextureSampler::WrapMethod::Clamp},
                {"repeat", Samplers::TextureSampler::WrapMethod::Repeat},
                {"mirrored_repeat", Samplers::TextureSampler::WrapMethod::MirroredRepeat},
            };
            auto m = methods.find(node.as<std::string>());

            assertNode(m != methods.end(), "Unknown wrap method: " + node.as<std::string>() + ", must be one of: clamp, repeat, mirrored_repeat");

            method = m->second;
        }
        return method;
    }

    Samplers::TextureSampler::FilterMethod &SceneDeserializer::deserialize(Samplers::TextureSampler::FilterMethod &method, const YAML::Node &node)
    {
        if (node)
        {
            static std::map<std::string, Samplers::TextureSampler::FilterMethod> methods = {
                {"nearest", Samplers::TextureSampler::FilterMethod::Nearest},
                {"linear", Samplers::TextureSampler::FilterMethod::Linear},
            };
            auto m = methods.find(node.as<std::string>());

            assertNode(m != methods.end(), "Unknown filter method: " + node.as<std::string>() + ", must be one of: nearest, linear");

            method = m->second;
        }
        return method;
    }

    Transform &SceneDeserializer::deserialize(Transform &transform, const YAML::Node &node)
    {
        if (node)
        {
            deserialize(transform.position, node["position"]);
            deserialize(transform.rotation, node["rotation"]);
            deserialize(transform.scale, node["scale"]);
        }
        return transform;
    }

    m::dvec3 &SceneDeserializer::deserialize(m::dvec3 &vec, const YAML::Node &node)
    {
        if (node)
        {
            switch (node.Type())
            {
            case YAML::NodeType::Sequence:
                assertNode(node.size() == 3, "Vector must have 3 elements");
                vec = {node[0].as<float>(), node[1].as<float>(), node[2].as<float>()};
                break;

            case YAML::NodeType::Map:
                assertNode(node.size() == 3, "Vector must have 3 elements");
                assertNode(node["x"], "X node not found");
                assertNode(node["y"], "Y node not found");
                assertNode(node["z"], "Z node not found");
                vec = {node["x"].as<float>(), node["y"].as<float>(), node["z"].as<float>()};
                break;

            default:
                assertNode(false, "Vector must be a sequence or a map");
            }
        }
        return vec;
    }

    m::dquat &SceneDeserializer::deserialize(m::dquat &quat, const YAML::Node &node)
    {
        if (node)
        {
            switch (node.Type())
            {
            case YAML::NodeType::Sequence:
                assertNode(node.size() == 4, "Quaternion must have 4 elements");
                quat = {
                    node[3].as<float>(),
                    node[0].as<float>(),
                    node[1].as<float>(),
                    node[2].as<float>(),
                };
                break;

            case YAML::NodeType::Map:
                assertNode(node.size() == 4, "Quaternion must have 4 elements");
                assertNode(node["w"], "W node not found");
                assertNode(node["x"], "X node not found");
                assertNode(node["y"], "Y node not found");
                assertNode(node["z"], "Z node not found");
                quat = {
                    node["w"].as<float>(),
                    node["x"].as<float>(),
                    node["y"].as<float>(),
                    node["z"].as<float>(),
                };
                break;

            default:
                assertNode(false, "Quaternion must be a sequence or a map");
            }
        }
        return quat;
    }

    m::Color<float> &SceneDeserializer::deserialize(m::Color<float> &color, const YAML::Node &node)
    {
        if (node)
        {
            switch (node.Type())
            {
            case YAML::NodeType::Sequence:
                assertNode(node.size() == 3, "Color must have 3 elements");
                color = {
                    node[0].as<float>(),
                    node[1].as<float>(),
                    node[2].as<float>(),
                };
                break;

            case YAML::NodeType::Map:
                assertNode(node.size() == 3, "Color must have 3 elements");
                assertNode(node["r"], "R node not found");
                assertNode(node["g"], "G node not found");
                assertNode(node["b"], "B node not found");
                color = {
                    node["r"].as<float>(),
                    node["g"].as<float>(),
                    node["b"].as<float>(),
                };
                break;

            case YAML::NodeType::Scalar:
            {
                auto bin = node.as<uint32_t>();
                color = {
                    ((bin >> 16) & 0xFF) / 255.0f,
                    ((bin >> 8) & 0xFF) / 255.0f,
                    (bin & 0xFF) / 255.0f,
                };
            }
            break;

            default:
                assertNode(false, "Color must be a sequence or a map");
            }
        }
        return color;
    }
} // namespace rt
