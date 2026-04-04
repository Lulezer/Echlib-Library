#include "graphics_internal.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>

namespace ech {

    // --- GLOBAL DEFINITIONS ---
    unsigned int vao = 0, vbo = 0, ebo = 0;
    unsigned int shaderProgramShape = 0;
    unsigned int shaderProgramTexture = 0;
    unsigned int shaderProgramText = 0;
    unsigned int textVAO = 0, textVBO = 0;

    glm::mat4 projection;
    glm::mat4 view;

    // --- SHADER SOURCES ---
    static const char* shapeVertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        uniform mat4 uProjection;
        uniform mat4 uView;
        void main() { gl_Position = uProjection * uView * vec4(aPos, 0.0, 1.0); }
    )";

    static const char* shapeFragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec4 uColor;
        void main() { FragColor = uColor; }
    )";

    static const char* textVertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        uniform mat4 uProjection;
        uniform mat4 uView;
        void main() {
            gl_Position = uProjection * uView * vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    static const char* textFragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoord;
        out vec4 FragColor;
        uniform sampler2D textAtlas;
        uniform vec3 textColor;
        void main() {
            float alpha = texture(textAtlas, TexCoord).r;
            if (alpha < 0.01) discard;
            FragColor = vec4(textColor, alpha);
        }
    )";

    static const char* textureFragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoord;
    uniform sampler2D texture1;
    void main() {
        vec4 texColor = texture(texture1, TexCoord);
        if (texColor.a < 0.1) discard;
        FragColor = texColor;
    }
)";

    // --- HELPERS ---
    static void CompileShader(unsigned int shader, const char* source, const char* label) {
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        int ok; glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char log[512]; glGetShaderInfoLog(shader, 512, nullptr, log);
            std::cerr << "Shader Error (" << label << "): " << log << std::endl;
        }
    }

    unsigned int CreateShaderProgram(const char* vsSrc, const char* fsSrc) {
        unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
        CompileShader(vs, vsSrc, "Vertex");
        unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
        CompileShader(fs, fsSrc, "Fragment");
        unsigned int prog = glCreateProgram();
        glAttachShader(prog, vs); glAttachShader(prog, fs);
        glLinkProgram(prog);
        glDeleteShader(vs); glDeleteShader(fs);
        return prog;
    }

    void InitGraphics(GLFWwindow* window) {
        assert(window && "Window was null in InitGraphics");
        glfwMakeContextCurrent(window);

        int w, h; glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        projection = glm::ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f);
        view = glm::mat4(1.0f);

        // 1. Shape & Texture Shared Setup
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao); // START RECORDING VAO

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 4096 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

        // CRITICAL FIX: Bind the EBO to this VAO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        // We don't need to put data in it yet, just bind it so the VAO "remembers" it.

        // Setup position (0) and UV (1)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0); // STOP RECORDING VAO

        // 2. COMPILE SHADERS (Only once each!)
        shaderProgramShape = CreateShaderProgram(shapeVertexShaderSource, shapeFragmentShaderSource);
        shaderProgramText = CreateShaderProgram(textVertexShaderSource, textFragmentShaderSource);
        shaderProgramTexture = CreateShaderProgram(textVertexShaderSource, textureFragmentShaderSource);

        // 3. Text Setup (Separate VAO for font rendering)
        glGenVertexArrays(1, &textVAO);
        glGenBuffers(1, &textVBO);

        glBindVertexArray(textVAO); // START RECORDING TEXT VAO
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferData(GL_ARRAY_BUFFER, 16384 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0); // STOP RECORDING TEXT VAO

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}
