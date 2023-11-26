<img src="./screenshots/screenshot_1.PNG" width="1000px">

# Vulkan render

## About

This repository contains a number of modules, that makes 3D render using [Vulkan API](https://www.vulkan.org/). All render moduls are implemented as static libraries and located in [core](core) directory, check READMEs in modules directories for more detailed information. Tests are located in [tests](tests) directory and implemented as executable applications, that demonstrate rendering functionality. Almost all build dependences could be cloning by [clone script](dependences/libs/clone.py), for more build informations check [requirements](#Requirements) and [build](#Build-tests-applications) paragraphs.

## Build tests applications

From working directory:

* Clone dependences:
```
python ./dependences/libs/clone.py
```
* Build:
```
mkdir build
cd build
cmake .. -DCUDA_RAY_TRACING=ON
cmake --build . --config Release
```

Use `CUDA_RAY_TRACING` flag for build `cudaRayTracing` module and `testCuda` test, make shure you have installed `CUDA`, it is not installed with dependences by `clone.py` script.

## Requirements

A number of dependences that could be cloning by [clone script](dependences/libs/clone.py):
* [stb](https://github.com/nothings/stb.git) - image loader
* [tinygltf](https://github.com/syoyo/tinygltf.git) - gltf model loader
* [tinyply](https://github.com/ddiakopoulos/tinyply.git) - ply model loader
* [vulkan headers](https://github.com/KhronosGroup/Vulkan-Headers.git) - Vulkan header files 
* [ImGui](https://github.com/ocornut/imgui.git) - open GUI library
* [glfw](https://github.com/glfw/glfw.git) - window API, automatically builded by script

Also you need installed `Vulkan API` and optional `CUDA`.

## Decription

At the moment, only deferred render (`deferredGraphics`) has been implemented. It's a stand-alone module that can be replaced with another implementation of render. In the future, it is planned to implement a simpler forward rendering as modul. Current implementation requires around 1.5-2 Gb of device memory for initialization and supporting all functionality.

All renders, for example `deferredGraphics`, `cudaRayTracing` or `imguiGraphics`, host by graphics manager (`graphicsManager`), that creates and supports all necessary stuff. Manager and render moduls communicats by linker classes, all instance of render moduls must implement oun linker class inherited from the linker interface.

`cudaRayTracing` is simpliest implementation of ray tracing, it has low perfomance. Possible in the future this modul will be improved. In current state it shows simultaneos `CUDA` and `Vulkan` work.

[ImGui](https://github.com/ocornut/imgui.git) was integrated as graphics modul `imguiGraphics`, and could be launched with other graphics modul. Generally speaking all graphics moduls could be launched simultaneosly, and show in different parts of window. Also, for example, two instance of graphics could be launched by single manager for multi-view presentation (`testPos`).

Supported models formats: gltf, glb, ply.
