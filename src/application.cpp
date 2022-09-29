#include <application.h>

#include <optional>
#include <resource_loaders.h>
#include <resources.h>
#include <rt_renderer.h>
#include <scene/scene_deserializer.h>
#include <scene/scene_serializer.h>
#include <stream_formatter.h>

namespace rt
{
    Application::Application(const std::filesystem::path &originPath)
        : originPath(originPath),
          threadPool(std::thread::hardware_concurrency() == 0 ? 1 : std::thread::hardware_concurrency()),
          resources(&threadPool, originPath / "resource"),
          scene(std::make_unique<Scene>(
              Camera(Transform(m::dvec3(0, 3.5, 7))) //
              )),
          renderThread(&threadPool),
          m_window(*this)
    {
        renderers.emplace("Raytracing", new RTRenderer());
        renderThread.setRenderer(renderers["Raytracing"].get());

        resources.add<Resources::VoxelGridResource>(new ResourceLoaders::VoxelGridLoader());
        resources.add<Resources::TextureResource>(new ResourceLoaders::TextureLoader());

        auto texture = resources += "UV_Test.png";

        auto envTex = resources += "belfast_farmhouse_4k.hdr";
        scene->environmentTexture = new Samplers::TextureSampler(envTex);

        resources += "castle.vox";
        resources += "monu1.vox";
        auto voxels = resources += "monu2.vox";
        resources += "monu3.vox";
        resources += "teapot.vox";

        auto defaultMaterial = scene->addMaterial(new Materials::LitMaterial("Default", std::make_unique<Samplers::ColorSampler>(m::Color<float>(1))));
        auto floorMaterial = scene->addMaterial(new Materials::LitMaterial("Floor", std::make_unique<Samplers::ColorSampler>(m::Color<float>(241, 196, 132) / 255.0f)));
        auto objectMaterial = scene->addMaterial(new Materials::LitMaterial("Objects", std::make_unique<Samplers::TextureSampler>(texture), 0.1f, 1, 1, 0.36f));
        auto voxelMaterial = scene->addMaterial(new Materials::LitMaterial("Voxels", std::make_unique<Samplers::PaletteSampler>(voxels), 0.1f, 1, 1, 0.36f));

        // scene->addShape(new Shapes::Sphere("Sphere 1", 0.3, Transform(m::dvec3(0.5, 0.5, 0)), objectMaterial));
        // scene->addShape(new Shapes::Sphere("Sphere 2", 0.3, Transform(m::dvec3(-0.5, 0.5, 0)), objectMaterial));
        // scene->addShape(new Shapes::Cube(Transform(m::dvec3(0, 0.9, 0), m::dquat(-0.022, 0.448, 0.629, 0.635), m::dvec3(0.45)), objectMaterial));
        // scene->addShape(new Shapes::Sphere(0.1, Transform(m::dvec3(0, 0, 0))));
        // scene->addShape(new Shapes::Sphere(0.1, Transform(m::dvec3(0.6, 1, 0.5))));
        scene->addShape(new Shapes::Plane(Transform(m::dvec3(0, 0, 0)), floorMaterial));
        scene->addShape(new Shapes::VoxelShape(voxels, Transform(m::dvec3(0, 3.5 * 1.75, 0), m::rotate(m::rotate(m::dquat(1, 0, 0, 0), m::radians(-90.0), m::dvec3(1, 0, 0)), m::radians(180.0), m::dvec3(0, 0, 1)), m::dvec3(7, 7, 7 * 1.75)), voxelMaterial));
        // scene->addLight(new Lights::PointLight{m::dvec3(0, 2, 0), m::Color<float>(1.0f), 1.27f});
        scene->addLight(new Lights::DirectionalLight(m::dvec3(0, 1, 0), m::Color<float>(255, 248, 208) / 255.0f, 0.5f));
        rtstd::formatterstream(std::cout) << *scene << std::endl;
    }

    Application::~Application()
    {
    }

    void Application::run()
    {
        bool running = true;
        while (running)
        {
            Event event = m_eventStream.get();
            switch ((EventType)event.index())
            {
            case EventType::CloseApplication:
                running = false;
                break;
            case EventType::LoadScene:
                try
                {
                    SceneDeserializer deserializer(&resources, originPath / "resource");

                    auto scene = deserializer.deserializeFile(std::get<Events::LoadScene>(event).path);

                    rtstd::formatterstream(std::cout) << *scene << std::endl;

                    renderThread.waitUntilFinished();
                    this->scene = std::unique_ptr<Scene>(scene);

                    *this << Events::Render();
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << '\n';
                }

                break;
            case EventType::SaveScene:
                try
                {
                    SceneSerializer serializer;
                    serializer.serialize(*scene, std::get<Events::SaveScene>(event).path);
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << '\n';
                }

                break;
            }
        }
    }
}