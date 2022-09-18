# Raytracing project
### Project for module "Programmier Paradigmen"

## Dependencies

- [CMake](https://cmake.org/)
- GCC or MSVC or every other C/C++ generator supported by CMake
- OpenGL 3.3
- C/C++ libs (self contained)
    - [GLFW](https://www.glfw.org/)
    - [Glad](https://glad.dav1d.de/)
    - [Dear ImGui](https://github.com/ocornut/imgui)
    - [STB Image](https://github.com/nothings/stb)
    - [GLM](https://glm.g-truc.net/0.9.9/index.html)
    - [yaml-cpp](https://github.com/jbeder/yaml-cpp)

## How to run

### In the terminal

1. Generate the project using CMake

From the source directory, run:
```bash
mkdir build
cd build
cmake ../
```
You can optionally specify which generator to use with the `-G` option, use `cmake --help` to get a list of available generators

2. Build the project

From the previously created build directory, run:
```bash
cmake --build .
```

3. Run the project

From the build directory, run:
```bash
./Ray_Tracer.exe
```
When using MSVC, the binarys are in a subfolder according to the specified configuration:
```bash
./Debug/Ray_Tracer.exe
./Release/Ray_Tracer.exe
```

### With VS Code

1. Open the source directory in VS Code
2. Install necessary extensions:
    - CMake Tools
    - C/C++
3. Select a generator in the bottom bar
4. Press `F5` to run

### With Visual Studio

1. Open the source directory in Visual Studio
2. Select the desired build configuration
3. Make sure to select `Ray_Tracer.exe` as start element
4. Press `F5` to run

