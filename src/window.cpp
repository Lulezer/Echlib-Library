#include "window.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

#include "internal.hpp"
#include "graphics_internal.hpp"

static bool s_GLFWInitialized = false;

namespace ech {

    Window::Window(int width, int height, const char* title)
        : m_Width(width), m_Height(height)
    {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW\n";
            return;
        }
         
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!m_Window) {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(m_Window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD\n";
            glfwDestroyWindow(m_Window);
            glfwTerminate();
            m_Window = nullptr;
            return;
        }

        glViewport(0, 0, width, height);

        // ? Register as default window if none exists
        auto*& def = ech::GetDefaultWindow();
        if (!def) def = this;

        // ? Graphics init must happen AFTER context exists
        ech::InitGraphics(m_Window);
		glfwSwapInterval(0); // Disable vsync by default
        if (!s_GLFWInitialized)
        {
            if (!glfwInit()) {
                std::cerr << "Failed to initialize GLFW\n";
                return;
            }
            s_GLFWInitialized = true;
        }
    }

    Window::~Window() {
        if (m_Window) {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }

        auto*& def = ech::GetDefaultWindow();
        if (def == this) def = nullptr;
    }

    bool Window::ShouldClose() const {
        return m_Window ? glfwWindowShouldClose(m_Window) : true;
    }

}
