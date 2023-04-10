# Micro RayTracer

This is a source code of a small CPU-based ray tracer using Dear ImGUI implemented in a simplified form with the Walnut framework by The Cherno and initially based on the The Cherno Ray Tracing youtube series. I slightly deviated in my implementation to provide for a more general solution. I will be implementing a complete GPU solution using Vulkan with the help of the following Vulkan Workshop

## Walnut Documentation by The Cherno

Walnut is a simple application framework built with Dear ImGui and designed to be used with Vulkan - basically this means you can seemlessly blend real-time Vulkan rendering with a great UI library to build desktop applications. The plan is to expand Walnut to include common utilities to make immediate-mode desktop apps and simple Vulkan applications.

Currently supports Windows - with macOS and Linux support planned. Setup scripts support Visual Studio 2022 by default.

### 3rd party libaries

- [Dear ImGui](https://github.com/ocornut/imgui)
- [GLFW](https://github.com/glfw/glfw)
- [stb_image](https://github.com/nothings/stb)
- [GLM](https://github.com/g-truc/glm) (included for convenience)

## Build for Windows

### Requirements

- [Visual Studio 2022](https://visualstudio.com) (not strictly required, however included setup scripts only support this)
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) (preferably a recent version)

### Getting Started

Once you've cloned, run `scripts/Setup.bat` to generate Visual Studio 2022 solution/project files. Once you've opened the solution, you can run the WalnutApp project to see a basic example (code in `WalnutApp.cpp`). I recommend modifying that WalnutApp project to create your own application, as everything should be setup and ready to go.


## Build for Linux [meson]

[Based on the following fork](https://github.com/neutrino2211/WalnutLinuxSanityCheck)

Install the following dependencies ```glm glfw3 vulkan``` using your platform's package manager then run ```scripts/setup.sh``` to copy the meson.build files to the right directories. Once done, you'll be ready to build your Walnut app using ```meson``` and ```ninja```.

```bash
mkdir bin
cd bin
meson ..
ninja

exec WalnutApp/Walnut
```

### Install Vulkan

- Tools:

    Debian `sudo apt install vulkan-tools`

    Fedora `sudo dnf install vulkan-tools`

    Arch `sudo pacman -S vulkan-tools`

- Dev packages

    Debian `sudo apt install libvulkan-dev`

    Fedora `sudo dnf install vulkan-loader-devel`

    Arch `sudo pacman -S vulkan-devel`

- Debug packages

    Debian `sudo apt install vulkan-validationlayers-dev spirv-tools`
    
    Fedora `sudo dnf install mesa-vulkan-devel vulkan-validation-layers-devel`

### Install GLFW

Debian `sudo apt install libglfw3-dev`

Fedora `sudo dnf install glfw-devel`

Arch `sudo pacman -S glfw-wayland # glfw-x11 for X11 users`

### Install GLM

Debian `sudo apt install libglm-dev`

Fedora `sudo dnf install glm-devel`

Arch `sudo pacman -S glm`

### In case `Xxf68vm` and `Xi` libraries are not yet installed on your system 

Debian `sudo apt install libxxf86vm-dev libxi-dev`

Fedora `sudo dnf install libXi libXxf86vm`

Arch `sudo pacman -S libxi libxxf86vm`