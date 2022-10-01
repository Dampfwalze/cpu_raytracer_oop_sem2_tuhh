# Final report

## See also

- [How to build](build.md)
- [Scene serialization spec](scene_serialization_spec.md)
- [Class diagram](class_diagram.md)

## Usage

The application can be used in a command line or with a GUI.

When running the application with the `--nogui` flag, it will render a specified scene, save the resulting image to a specified file and terminate. 
In this mode the host system does not need GUI support to run the application.

There are a number of command line arguments you can specify. You can use `--help` to get a list of all available arguments. These include 
- `--scene` to specify the scene file
- `--output` to specify the output file
- `--width` and `--height` to specify the resolution of the output image 
- `--nogui` to run the application without a GUI

Every path can be specified absolute or relative, to the current working directory or relative, to the directory of the executable. Thats because, there are many resources and examples shipped with this application.

### GUI

There are 5 different pannels:

#### Viewport:

The **Viewport** lets you see the render result. 

If you hold right click on the viewport, you can move the camera with `w` and `a` to go back and forth, `s` and `d` to move sideways and `q` and `e` to move up and down. Rotate it by dragging the mouse.

If you click on the viewport with the left mouse button, a marker will be placed at that position. This is the pixel, that will be recorded by the `PixelLogger`.

#### Control:

The **Control** pannel is host to the buttons, that control the rendering.

- `Render` starts the rendering process. It is also run, when any parameter changes.
- `Profile` starts the rendering process with profiling enabled.

These only control the rendering inside the application. To save the resukt to disk, the Output section is used:

Select an output location by clicking on `Browse`. Now you can hit `Render output` and the result will be saved imidiately after rendering has finished.

Parameters:
- Tile size: The size of the tiles, in which the image is divided for parralization.
- Mixing factor: This factor is used in the mixing process of colors. Since the color range is not bounded when rendering, artifacts can occur with the color mixing (for example when the light intensity is to high). To prevent that, you can increase this factor. Every color is divided with it before mixing and the result is multiplied with it again. 
- Recursion depth: The maximum recursion depth for the ray tracer. This is the maximum number of reflections, that are traced.
- [Tone mapping algorithm](../src/rt_renderer.cpp#L29): In the resulting image, the colors are not bounded. But since the output should be bounded, we need to map every color to the output range. To accomplish that, there are many different algorithms. There are 3 different algorithms implemented:
    - `None`: The colors are clamped to the output range, loosing every detail above the maximum and below the minimum of the output range.
    - `Reinhard`: Every color component is divided by the sum of itself and 1
    - `Exposure`: Every color component is calculated with the formula `1 - e^(-c * e)`, where `c` is the color component and `e` is the exposure parameter.
- Exposure: The exposure parameter for the exposure tone mapping algorithm.
- [Gamma correction](../src/rt_renderer.cpp#L39): It is used to change the overall brightness of the image.
    - Gamma: The gamma value used for gamma correction.
    - Scale: The scale value used for gamma correction.

The Control panel also contains the output of the `PixelLogger`.

#### Profiler:

The **Profiler** pannel shows the profiling results of the last rendering process. You can zoom in and out by scrolling and move the view by dragging the mouse.

#### Resources:

The **Resources** pannel shows all the resources, that you can be used in the scene. Hover over them to see details like the path. You can use drag and drop to add them to the scene in the appropriate parameter fields.

#### Scene inspector:

The **Scene inspector** lets you edit the scene. You can change most parameters of any `SceneObject`. For resources, you can use drag and drop to change it.

#### Context menu:

From here, you can open, save and create a new scenes.

#### Dear ImGui Demo:

This is a demo of the [GUI library](https://github.com/ocornut/imgui), that is used in this application. This is not part of this application, but feel free, to explore it and appreciate this nice library. 