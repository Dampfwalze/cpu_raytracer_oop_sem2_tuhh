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

        loadDefaultScene();
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
                    loadScene(std::get<Events::LoadScene>(event).path);

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

    void Application::loadScene(const std::filesystem::path &path)
    {
        static SceneDeserializer deserializer(&resources, originPath / "resource");

        auto scene = deserializer.deserializeFile(path);

        renderThread.waitUntilFinished();
        this->scene = std::unique_ptr<Scene>(scene);
    }

    void Application::loadDefaultScene()
    {
        scene = std::make_unique<Scene>(
            Camera(Transform(m::dvec3(0.820, 2.694, 5.989), {0.997, -0.079, 0.012, 0.001})) //
        );

        resources += "UV_Test.png";
        auto texture = resources += "box.jpg";

        auto envTex = resources += "belfast_farmhouse_4k.hdr";
        scene->environmentTexture = new Samplers::TextureSampler(envTex);

        resources += "castle.vox";
        resources += "monu1.vox";
        resources += "monu2.vox";
        resources += "monu3.vox";
        resources += "teapot.vox";
        auto voxels = resources += "chr_knight.vox";

        auto defaultMaterial = scene->addMaterial(new Materials::LitMaterial("Default", std::make_unique<Samplers::ColorSampler>(m::Color<float>(1))));
        auto floorMaterial = scene->addMaterial(new Materials::LitMaterial("Floor", std::make_unique<Samplers::ColorSampler>(m::Color<float>(241, 196, 132) / 255.0f), 0.03, 1, 1, 0.01));
        auto boxMaterial = scene->addMaterial(new Materials::LitMaterial("Box", std::make_unique<Samplers::TextureSampler>(texture), 0.1f, 1, 1, 0.36f));
        auto sphereMaterial = scene->addMaterial(new Materials::LitMaterial("Sphere", std::make_unique<Samplers::ColorSampler>(), 0.0f, 0.2, 0.6, 1));
        auto voxelMaterial = scene->addMaterial(new Materials::LitMaterial("Voxels", std::make_unique<Samplers::PaletteSampler>(voxels), 0.1f, 1, 1, 0.36f));

        scene->addShape(new Shapes::Plane(Transform(), floorMaterial));
        scene->addShape(new Shapes::Sphere(1, Transform(m::dvec3(-0.5, 1.1, -0.2)), sphereMaterial));
        scene->addShape(new Shapes::Cube(Transform(m::dvec3(2.15, 0.75, 0.33), m::dquat(0.32246212508824196, 0, 0.9465823671945168, 0), m::dvec3(1.5)), boxMaterial));

        scene->addLight(new Lights::DirectionalLight(m::dvec3(0.663, 0.608, -0.438), m::Color<float>(255, 248, 208) / 255.0f, 0.5f));
    }
}