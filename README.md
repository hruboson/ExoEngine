# ExoEngine

3D graphics engine built on the Vulkan API.

<img src="exo.png" alt="ExoEngine icon" width="100"/>

# Build

To build the engine you need following (for Windows):

* MinGW64
    *It is also posible to use different compiler (MSVC for example), but you would have to change CMakeLists.txt*

    - I'm using MSYS:
        - Download [MSYS](https://www.msys2.org)
        - Run following commands:
            - pacman -Syu
            - pacman -Su
            - pacman -S mingw-w64-x86_64-{toolchain,cmake,glfw,vulkan-headers,vulkan-loader,vulkan-validation-layers,libsqlite,SDL2,magnum-integration}
* [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows)
* [CMake](https://cmake.org/download)
* Set system environment variables for MinGW, CMake and Vulkan
    - X:\MSYS\mingw64\bin
    - X:\CMake\bin
    - X:\Vulkan\1.3.224.1\Bin

Set variables in ` `.env.cmake` ` to match your system
Run ` `.\build.bat` ` command from top-most folder
**You might need to remove the existing build folder before running build for the first time**

I will be testing this build tutorial more, but I hope I haven't forgotten anything. If you have problem, contact me or create an Issue.

# Purpose

This engine was built for my own study purposes. I'm still developing it when I find the time. Tips and critiques are welcome

# Documentation

I made quite nice documentation/paper for this project. The problem is it was for my school so it's in czech language, so I'm not gonna post it here.
But thorough english documentation is planned, so stay tuned.

# FYI

If you want to copy, study or do anything else with the code you are welcome. I will not allow any pull requests as this isn't really "public project", but rather my studying material. You are free to fork it and do anything you want with it though.

## Dependencies

The engine is built mainly on Vulkan API and other additional libraries.

#### Additional libraries

* TinyObjectLoader
* STB library
* GLFW
* GLM
* SQLite
* ImGui
