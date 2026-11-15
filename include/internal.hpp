#pragma once
#include <chrono>
#include "Window/window.hpp"

namespace ech {
    Window*& GetDefaultWindow();
    extern float deltatime;
    extern std::chrono::high_resolution_clock::time_point lastFrameTime;
}