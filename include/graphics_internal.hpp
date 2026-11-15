#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// shader sources (declarations only)
extern const char* shapeVertexShaderSource;
extern const char* shapeFragmentShaderSource;
extern const char* textureVertexShaderSource;
extern const char* textureFragmentShaderSource;
extern const char* textFragmentShaderSource;

namespace ech {
    // Internal renderer objects (declared here so echlib.cpp can use them)
    extern unsigned int vao, vbo, ebo;
    extern unsigned int shaderProgramShape, shaderProgramTexture, shaderProgramText;
    extern unsigned int textVAO, textVBO;
    extern glm::mat4 projection;
    extern glm::mat4 view;

    extern unsigned int shaderProgramShape;



    unsigned int CreateShaderProgram(const char* vertexSrc, const char* fragmentSrc);
    void InitGraphics(GLFWwindow* window);

    // Called each frame by StartDrawing
    void UploadShapeUniforms();
}
