#pragma once
#include <string>
#include <glm/glm.hpp>


struct GLFWwindow;

namespace ech {

    class Window {
    public:
        Window(int width, int height, const char* title);
        ~Window();

        bool ShouldClose() const;

        GLFWwindow* GetNative() const { return m_Window; }
         
        int Width() const { return m_Width; }
        int Height() const { return m_Height; }

        const glm::mat4& Projection() const { return m_Projection; }
        const glm::mat4& View() const { return m_View; }
        GLFWwindow* GetNativeHandle() const { return m_Window; }


    private:
        GLFWwindow* m_Window = nullptr;
        int m_Width = 0;
        int m_Height = 0;

        glm::mat4 m_Projection{};
        glm::mat4 m_View{};

    };

}
