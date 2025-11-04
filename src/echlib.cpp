#include "Echlib.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <chrono>

// stb_image for textures
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

// glm for matrices
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <filesystem>

namespace ech {

    // === GLOBALS ===
    GLFWwindow* window = nullptr;
    int windowWidth = 0;
    int windowHeight = 0;

    unsigned int vao = 0, vbo = 0, ebo = 0;
    unsigned int shaderProgramShape = 0;
    unsigned int shaderProgramTexture = 0;

    std::unordered_map<int, bool> mouseButtonPreviousStates;
    std::unordered_map<int, bool> keyPreviousStates;

    constexpr float PI = 3.14159265359f;

 
    // Timing
    static std::chrono::high_resolution_clock::time_point lastFrameTime = std::chrono::high_resolution_clock::now();
    static float deltaTime = 0.0f;


       
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec2 cameraPos(0.0f, 0.0f);


    // === SHADERS ===
    // Shape Vertex Shader
    static const char* shapeVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 uProjection;
uniform mat4 uView;

void main() {
    gl_Position = uProjection * uView * vec4(aPos, -0.5, 1.0);
}

)";


    // Shape Fragment Shader
    static const char* shapeFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

uniform vec4 uColor; // Color of the shape

void main() {
    FragColor = uColor; // Output the color of the shape
}

)";


    // Texture Vertex Shader
    static const char* textureVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;       // Position of the vertex
layout (location = 1) in vec2 aTexCoord;  // Texture coordinates

out vec2 TexCoord;

uniform mat4 uProjection; // Projection matrix for 2D rendering
uniform mat4 uView;       // Add this if you also want camera/view support

void main() {
    gl_Position = uProjection * uView * vec4(aPos, -0.5, 1.0);
    TexCoord = aTexCoord;  // Pass texture coordinates to fragment shader
}

)";


    // Texture Fragment Shader
    static const char* textureFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1; // The texture for rendering

void main() {
    vec4 texColor = texture(texture1, TexCoord);
    if (texColor.a < 0.1) discard; // Discard transparent pixels
    FragColor = texColor; // Output the texture color
}

)";

    // === HELPERS ===
    void CompileShader(unsigned int shader, const char* source, const std::string& type) {
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "ERROR::" << type << " SHADER COMPILATION FAILED\n" << infoLog << std::endl;
        }
    }

    unsigned int CreateShaderProgram(const char* vertexSource, const char* fragmentSource) {
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        CompileShader(vertexShader, vertexSource, "Vertex");

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        CompileShader(fragmentShader, fragmentSource, "Fragment");

        unsigned int shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        int success;
        char infoLog[512];
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER PROGRAM LINKING FAILED\n" << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return shaderProgram;
    }

    void InitGraphics() {
        // Create VAO / VBO / EBO for dynamic drawing
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // reserve some space (will rebuffer per draw)
        glBufferData(GL_ARRAY_BUFFER, 1024 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 1024 * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        shaderProgramShape = CreateShaderProgram(shapeVertexShaderSource, shapeFragmentShaderSource);
        shaderProgramTexture = CreateShaderProgram(textureVertexShaderSource, textureFragmentShaderSource);
    }

    // --- Window / Context ---
    void CreateWindow(int width, int height, const char* title) {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW\n";
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(window);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD\n";
            glfwDestroyWindow(window);
            glfwTerminate();
            return;
        }

        windowWidth = width;
        windowHeight = height;
        glViewport(0, 0, width, height);

        // Update projection on resize
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int w, int h) {
            windowWidth = w;
            windowHeight = h;
            glViewport(0, 0, w, h);
            projection = glm::ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f);
            });

        InitGraphics();

        // initial projection/view
        projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
        view = glm::mat4(1.0f);
    }

    void CloseWindow() {
        if (window) glfwDestroyWindow(window);
        glfwTerminate();
    }

    int WindowShouldClose() {
        return window ? glfwWindowShouldClose(window) : 1;
    }

    // Timing
    float GetDeltaTime() {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastFrameTime;
        lastFrameTime = currentTime;
        deltaTime = elapsed.count();
        return deltaTime;
    }

    // Frame control
    void StartDrawing() {
        glClear(GL_COLOR_BUFFER_BIT);

        // Set for shape shader
        glUseProgram(shaderProgramShape);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramShape, "uProjection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramShape, "uView"), 1, GL_FALSE, &view[0][0]);

        // Set for texture shader
        glUseProgram(shaderProgramTexture);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramTexture, "uProjection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramTexture, "uView"), 1, GL_FALSE, &view[0][0]);
    }


    void EndDrawing() {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void ClearBackground(Color color) {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    // --- DRAW FUNCTIONS ---
    void DrawLine(float x1, float y1, float x2, float y2, Color color) {
        float vertices[] = {
            x1, y1,
            x2, y2
        };

        glUseProgram(shaderProgramShape);
        glUniform4f(glGetUniformLocation(shaderProgramShape, "uColor"), color.r, color.g, color.b, color.a);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_LINES, 0, 2);
    }

    void DrawTriangle(float x, float y, float w, float h, Color color) {
        float vertices[] = {
            x,     y,
            x + w, y,
            x + w / 2.0f, y + h
        };

        glUseProgram(shaderProgramShape);
        glUniform4f(glGetUniformLocation(shaderProgramShape, "uColor"), color.r, color.g, color.b, color.a);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void DrawRectangle(float x, float y, float w, float h, Color color) {
        float vertices[] = {
            x,     y,
            x + w, y,
            x + w, y + h,
            x,     y + h
        };
        unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

        glUseProgram(shaderProgramShape);

        // Send matrices here
        

        glUniform4f(glGetUniformLocation(shaderProgramShape, "uColor"), color.r, color.g, color.b, color.a);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }


    void DrawCircle(float x, float y, float radius, Color color) {
        const int numSegments = 64;
        std::vector<float> verts;
        verts.reserve((numSegments + 2) * 2);

        // center
        verts.push_back(x);
        verts.push_back(y);

        for (int i = 0; i <= numSegments; ++i) {
            float angle = 2.0f * PI * i / numSegments;
            float vx = x + radius * cos(angle);
            float vy = y + radius * sin(angle);
            verts.push_back(vx);
            verts.push_back(vy);
        }

        glUseProgram(shaderProgramShape);
        glUniform4f(glGetUniformLocation(shaderProgramShape, "uColor"), color.r, color.g, color.b, color.a);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)(numSegments + 2));
    }

    unsigned int LoadTexture(const char* path) {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Texture params
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(false); // we use projection flipped; keep consistent
        unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = GL_RGB;
            if (nrChannels == 1) format = GL_RED;
            else if (nrChannels == 3) format = GL_RGB;
            else if (nrChannels == 4) format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cerr << "Failed to load texture: " << path << std::endl;
        }
        stbi_image_free(data);
        return textureID;
    }

    void DrawTexturedRectangle(float x, float y, float w, float h, unsigned int textureID) {
        // Vertex format: x, y, u, v
        float vertices[] = {
            x,     y,     0.0f, 1.0f,
            x + w, y,     1.0f, 1.0f,
            x + w, y + h, 1.0f, 0.0f,
            x,     y + h, 0.0f, 0.0f
        };

        unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

        glUseProgram(shaderProgramTexture);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

        // pos
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // tex
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    // --- Input ---
    static int TranslateKey(int key) {
        if (key >= KEY_A && key <= KEY_Z)
            return GLFW_KEY_A + (key - KEY_A);

        switch (key) {
        case KEY_SPACE: return GLFW_KEY_SPACE;
        case KEY_ESCAPE: return GLFW_KEY_ESCAPE;
        case KEY_ENTER: return GLFW_KEY_ENTER;
        default: return GLFW_KEY_UNKNOWN;
        }
    }

    static int TranslateMouseButton(int button) {
        switch (button) {
        case MOUSE_LEFT_BUTTON:   return GLFW_MOUSE_BUTTON_LEFT;
        case MOUSE_RIGHT_BUTTON:  return GLFW_MOUSE_BUTTON_RIGHT;
        case MOUSE_MIDDLE_BUTTON: return GLFW_MOUSE_BUTTON_MIDDLE;
        default: return -1;
        }
    }

    int IsKeyPressed(int key) {
        if (!window) return 0;
        return glfwGetKey(window, TranslateKey(key)) == GLFW_PRESS;
    }

    int IsKeyHeld(int key) {
        if (!window) return 0;
        int state = glfwGetKey(window, TranslateKey(key));
        return state == GLFW_REPEAT || state == GLFW_PRESS;
    }

    int IsMouseButtonPressed(int button) {
        if (!window) return 0;
        int glfwButton = TranslateMouseButton(button);
        if (glfwButton == -1) return 0;
        return glfwGetMouseButton(window, glfwButton) == GLFW_PRESS;
    }

    int IsMouseButtonHeld(int button) {
        if (!window) return 0;
        int glfwButton = TranslateMouseButton(button);
        if (glfwButton == -1) return 0;
        return glfwGetMouseButton(window, glfwButton) == GLFW_PRESS;
    }

    void UpdateCamera(float targetX, float targetY, float lerpFactor, float screenWidth, float screenHeight) {
        // Smooth follow
        camera.x += (targetX - camera.x) * lerpFactor;
        camera.y += (targetY - camera.y) * lerpFactor;

        // Build view matrix: translate only
        view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(-camera.x + screenWidth / 2.0f, -camera.y + screenHeight / 2.0f, 0.0f));
    }

    void ProUpdateCamera(float targetX, float targetY, float lerpFactor, float screenWidth, float screenHeight, float zoom = 1.0f) {
        // Smooth follow
        camera.x += (targetX - camera.x) * lerpFactor;
        camera.y += (targetY - camera.y) * lerpFactor;

        // Build view matrix
        glm::mat4 t = glm::mat4(1.0f);

        // 1️⃣ Move world so camera is centered
        t = glm::translate(t, glm::vec3(-camera.x + screenWidth / 2.0f, -camera.y + screenHeight / 2.0f, 0.0f));

        // 2️⃣ Apply zoom
        if (zoom != 1.0f)
            t = glm::scale(t, glm::vec3(zoom, zoom, 1.0f));

        view = t;
    }

    bool ech::CheckCollision(float ax, float ay, float aw, float ah,
        float bx, float by, float bw, float bh) {
        return (ax < bx + bw) &&
            (ax + aw > bx) &&
            (ay < by + bh) &&
            (ay + ah > by);
    }

    bool WriteFile(const std::string& path, const std::string& content) {
        std::ofstream file(path, std::ios::out | std::ios::trunc);
        if (!file.is_open()) return false;
        file << content;
        return true;
    }

    bool AppendFile(const std::string& path, const std::string& content) {
        std::ofstream file(path, std::ios::out | std::ios::app);
        if (!file.is_open()) return false;
        file << content;
        return true;
    }

    std::string ReadFile(const std::string& path) {
        std::ifstream file(path, std::ios::in);
        if (!file.is_open()) return "";
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return content;
    }

    bool FileExists(const std::string& path) {
        return std::filesystem::exists(path);
    }

    bool DeleteFile(const std::string& path) {
        return std::filesystem::remove(path);
    }




} // namespace ech
