# Preleminary Conception Design

See also:
- [Class diagram](class_diagram.md)
- [Scene Serialization Spec](scene_serialization_spec.md)

## Usage

The application can be used in a command line or with a GUI.

### Command line

When running the application with the `--nogui` flag, it will render a specified scene, save the resulting image to a specified file and terminate. These and other parameters can be specified in the command line:

```bash	
Ray_Tracer.exe --scene <scene file> --output <output file> --width <width> --height <height> --nogui
```

In this mode the host system does not need GUI support to run the application

The scene file format is explained in the [Scene Serialization Spec](scene_serialization_spec.md).

In this project, the viewpoint is simply called the `Camera`, observing a scene. Since it is part of the scene, it is defined in the scene file.

### GUI

When running the application without the `--nogui` flag, it will run with a GUI, that allows the user to manipulate the scene and render the result directly inside the application. The scene can also be saved to disk. Any commandline argument will be used for the respective part of the application. For example, when specifying a scene, it will be loaded.

## Application structure

[Class diagram](class_diagram.md#application)

The entry point is responsible for parsing commandline arguments, creating an instance of the application class and calling `run()` on it. This method is the main loop of the application and occupies the main thread for the lifetime of the application.

The Application class is responsible for creating all subsystems, namely the `WindowThread`, `RenderThread`, `ThreadPool`, `ResourceContainer` and `Renderer`. It is also the owner of the `Scene` and the `FrameBuffer`. To communicate to the main thread, an `EventStream` is used. The main loop is expressed by the following pseudocode:

```cpp
void Run() {
    while(true) {
        Event event = m_eventStream.get(); // Waits until a new event is available
        switch (event.type)
        {
            case EventType::Quit:
                return;
            case EventType::Render:
                ...
        }
    }
}
```

### WindowThread

The `WindowThread` is responsible for creating the window and handling all window events. It is encapsulated from the rest of the application, but is able to read any data, that it is responible to visualize in the GUI. It can trigger events to the application.

### RenderThread

The `RenderThread` is responsible for rendering the scene. It has an `EventStream` that can be used by the application, to trigger render events. It uses a `Renderer` to render the scene. The `Renderer` gets passed a `ThreadPool`, that can be used to parralize the rendering process.

### Rendering

The rendering process is parralized using a `ThreadPool`. The `ThreadPool` works through rendering tasks, each rendering a subtile of the hole image.

### Resources

[Class diagram](class_diagram.md#resource-container)

The `ResourceContainer` is responsible for loading and storing all resources. It loads its resources lazily, when they are requested. For this, when adding a resource, it returnes a weak pointer to the resource. When it is converted to a strong pointer, the resource gets loaded. This is also the time, when the type of the resource will be determained, based on the C++ Type the resource is accessed with. The type might be determained falsely, in which case, it will not be loaded. When it is accessed again with a different type, it will be attemted again, to load it.

## Notes

- For representing rotations, Quaternions are used. They can be converted into a rotation matrix easily.
- A projection matrix should be used, to achieve perspective. They are used in common 3D rendering techniques.

## Development steps

- Create the application structure (`Application`, `WindowThread`, `RenderThread`, `ThreadPool`).
  The GUI will be developed parallel from now on. It should support the development process of the renderer
- Create a renderer, rendering every pixel in parrallel. Maybe try to render some simple pattern (like a chess board)
- Try to render the first sphere, at the origin, binary color (sphere, no sphere)
- Create the Scene class with just shapes and transformations
- Achive perspective projection
- Add a camera to transform the scene
- Add materials with a fiexed lightsource
- Add lights to the scene
- Add the `ReourceContainer`
- Add samplers and ways to sample textures
- Add scene serialization
