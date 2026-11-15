# Echlib

Open-source C++ 2D game library (OpenGL + GLFW). CMake-first — dependencies live in `thirdparty/`.

## Requirements

- **CMake** 3.16+
- **C++17** compiler (Visual Studio 2022, or Clang/GCC on other platforms)
- **Windows** is what I develop on; other OS support is planned

## Build the library only

```powershell
cmake -S . -B build
cmake --build build --config Release
```

The static library ends up under `build/lib/` (`echlib.lib` on MSVC).

## Use in your own project

Do **not** copy only `include/` and `src/` — you still need GLFW, GLAD, glm, stb, etc.

**Recommended:** add Echlib as a submodule and pull it in with CMake:

```powershell
git submodule add https://github.com/Lulezer/Echlib-Library.git thirdparty/echlib
```

In your `CMakeLists.txt`:

```cmake
add_subdirectory(thirdparty/echlib "${CMAKE_BINARY_DIR}/echlib")
add_executable(MyGame src/main.cpp)
target_link_libraries(MyGame PRIVATE echlib)
if(WIN32)
    target_link_libraries(MyGame PRIVATE opengl32)
endif()
```

A minimal consumer template is in the sibling folder `echlib-starter/` locally; push it as `Lulezer/echlib-starter` (see `docs/GITHUB_SYNC.md`).

Override the library path without a submodule:

```powershell
cmake -S . -B build -DECHLIB_ROOT="C:/path/to/echlib"
```

## Quick API smoke test

```cpp
#include "echlib.hpp"

int main() {
    ech::CreateWindow(800, 600, "Hello");
    while (!ech::WindowShouldClose()) {
        ech::StartDrawing();
        ech::ClearBackground(ech::DARK_GRAY);
        ech::EndDrawing();
    }
    ech::CloseWindow();
}
```

## Resources

Runtime assets (when you use file loading) default to the `resources/` folder next to this repo in dev builds. Turn on `PRODUCTION_BUILD` in `CMakeLists.txt` for paths relative to the executable.

## Links

- [Releases](https://github.com/Lulezer/Echlib-Library/releases)
- [Wiki](https://github.com/Lulezer/Echlib-Library/wiki) (in progress)

## Credits

- [MrLLGD / Low Level Game Dev](https://www.youtube.com/@lowlevelgamedev9330) — early CMake/OpenGL guidance (project layout inspired by [GLFWCMakeSetup](https://github.com/meemknight/GLFWCMakeSetup))
- [raysan5](https://github.com/raysan5) — raudio