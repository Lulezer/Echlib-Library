#include "echlib.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <unordered_map>

// Global variables
GLFWwindow* window = nullptr;
unsigned int vao, vbo, ebo;
unsigned int shaderProgram;

int targetFps;  // Default FPS

// Vertex Shader Source
const char* vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec2 aPos;
    void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
    }
)";

// Fragment Shader Source
const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec4 uColor;
    void main() {
        FragColor = uColor;
    }
)";

// Function to compile shaders
void CompileShader(unsigned int shader, const char* source, const char* type) {
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR: " << type << " Shader Compilation Failed\n" << infoLog << std::endl;
    }
}

// Function to create shader program
void CreateShaderProgram() {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    CompileShader(vertexShader, vertexShaderSource, "Vertex");

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    CompileShader(fragmentShader, fragmentShaderSource, "Fragment");

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR: Shader Program Linking Failed\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

// Initialize graphics
void InitGraphics() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    CreateShaderProgram();
}

// Create the window
void MakeWindow(int width, int height, const char* title) {
    if (!glfwInit()) {
        std::cerr << "ERROR: Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "ERROR: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "ERROR: Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return;
    }

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int w, int h) { glViewport(0, 0, w, h); });

    InitGraphics();
}

void SetTargetFps(int target) {
    targetFps = target;
}

// Close the window
void CloseWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

// Check if the window should close
int WindowShouldClose() {
    return glfwWindowShouldClose(window);
}

// Start drawing
void StartDrawing() {
    glClear(GL_COLOR_BUFFER_BIT);
}

// End drawing
void EndDrawing() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

// Clear the background with a color
void ClearBackground(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

// Draw a triangle
void DrawTriangle(float x, float y, float width, float height, float r, float g, float b, float a) {
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

    float flippedY = windowHeight - y;

    float vertices[] = {
        (x / windowWidth) * 2.0f - 1.0f, 1.0f - (flippedY / windowHeight) * 2.0f,
        ((x + width) / windowWidth) * 2.0f - 1.0f, 1.0f - (flippedY / windowHeight) * 2.0f,
        ((x + width / 2) / windowWidth) * 2.0f - 1.0f, 1.0f - ((flippedY - height) / windowHeight) * 2.0f
    };

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(shaderProgram);
    glUniform4f(glGetUniformLocation(shaderProgram, "uColor"), r, g, b, a);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
}

// Draw a square
void DrawSquare(float x, float y, float width, float height, float r, float g, float b, float a) {
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

    float flippedY = windowHeight - y;

    float vertices[] = {
        (x / windowWidth) * 2.0f - 1.0f, 1.0f - (flippedY / windowHeight) * 2.0f,
        ((x + width) / windowWidth) * 2.0f - 1.0f, 1.0f - (flippedY / windowHeight) * 2.0f,
        ((x + width) / windowWidth) * 2.0f - 1.0f, 1.0f - ((flippedY - height) / windowHeight) * 2.0f,
        (x / windowWidth) * 2.0f - 1.0f, 1.0f - ((flippedY - height) / windowHeight) * 2.0f
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(shaderProgram);
    glUniform4f(glGetUniformLocation(shaderProgram, "uColor"), r, g, b, a);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

// Input System

std::unordered_map<int, bool> keyPreviousStates;

int IsKeyPressed(int key) {
    // Initialize the key state if it doesn't exist in the map
    if (keyPreviousStates.find(key) == keyPreviousStates.end()) {
        keyPreviousStates[key] = false;  // Set the initial state to not pressed
    }

    // Get the current state of the key
    bool currentState = glfwGetKey(window, key) == GLFW_PRESS;

    // Get the previous state of the key
    bool previousState = keyPreviousStates[key];

    // Update the key state in the map
    keyPreviousStates[key] = currentState;

    // Return true if the key was just pressed (not pressed last frame, pressed now)
    return currentState && !previousState;
}

// Detect if a key is being held down (continuously pressed)
int IsKeyHeld(int key) {
    // Initialize the key state if it doesn't exist in the map
    if (keyPreviousStates.find(key) == keyPreviousStates.end()) {
        keyPreviousStates[key] = false;  // Set the initial state to not pressed
    }

    return glfwGetKey(window, key) == GLFW_PRESS;
}
