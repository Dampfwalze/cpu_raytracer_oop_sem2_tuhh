# How to build

## Build dependencies

- [CMake](https://cmake.org/)
- GCC or MSVC
- [Doxygen](https://www.doxygen.nl/index.html) (Optional)

> **_NOTE:_** This project uses compiler features supported by GCC 10 or higher

## Download binaries

You can download the latest release from the [Release page](https://github.com/Dampfwalze/cpu_raytracer_oop_sem2_tuhh/releases).

## In the terminal

From the source directory, run:

```bash
mkdir build
cd build
cmake ../
```

You can optionally specify which generator to use with the `-G` option, use `cmake --help` to get a list of available generators.

You can enable the documentation build with `-DBUILD_DOC=ON`.
Documentation will be generated with [Doxygen](https://www.doxygen.nl/index.html) in the `doc` folder.

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

When using MSVC, the binaries are in a subfolder according to the specified configuration:

```bash
./Debug/Ray_Tracer.exe
./Release/Ray_Tracer.exe
```

## With VS Code

1. Open the source directory in VS Code
   (This can be done from the terminal using `code .` in CMD, PowerShell or bash, for example when using Wsl)
2. Install necessary extensions:
   - CMake Tools
   - C/C++
3. Select a generator in the bottom bar
4. Press `F5` to debug or `Ctrl+F5` to run. Alternatively, you can use the `Run` and `Debug` buttons in the bottom bar, provided by the cmake tools extension

## With Visual Studio

1. Open the source directory in Visual Studio
2. Select the desired build configuration
3. Make sure to select `Ray_Tracer.exe` as start element
4. Press `F5` to run
