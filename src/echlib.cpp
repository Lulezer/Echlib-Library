#include "echlib.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Global variables
GLFWwindow* window = nullptr;
unsigned int shapeVAO, shapeVBO, shapeEBO;  // For shapes
unsigned int textVAO, textVBO;              // For text
unsigned int shaderProgram;
unsigned int textShaderProgram;
int targetFps;

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


// Text Vertex Shader
const char* textVertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec2 aPos;
    layout(location = 1) in vec2 aTexCoord;
    out vec2 TexCoord;
    uniform mat4 projection; // Add projection matrix
    void main() {
        gl_Position = projection * vec4(aPos, 0.0, 1.0);
        TexCoord = aTexCoord;
    }
)";

// Text Fragment Shader
const char* textFragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoord;
    out vec4 FragColor;
    uniform sampler2D textTexture;
    uniform vec3 textColor;
    void main() {
        float alpha = texture(textTexture, TexCoord).r; // Use .r because we used GL_RED
        FragColor = vec4(textColor, alpha);
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

    int success;
    char infoLog[512];

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    CompileShader(vertexShader, vertexShaderSource, "Vertex");

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    CompileShader(fragmentShader, fragmentShaderSource, "Fragment");

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    unsigned int textVertexShader = glCreateShader(GL_VERTEX_SHADER);
    CompileShader(textVertexShader, textVertexShaderSource, "Text Vertex");

    unsigned int textFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    CompileShader(textFragmentShader, textFragmentShaderSource, "Text Fragment");

    textShaderProgram = glCreateProgram();
    glAttachShader(textShaderProgram, textVertexShader);
    glAttachShader(textShaderProgram, textFragmentShader);
    glLinkProgram(textShaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shape Shader Link Error: " << infoLog << std::endl;
    }

    // For text shader
    glGetProgramiv(textShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(textShaderProgram, 512, nullptr, infoLog);
        std::cerr << "Text Shader Link Error: " << infoLog << std::endl;
    }

    // Cleanup
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(textVertexShader);
    glDeleteShader(textFragmentShader);
}

// Initialize graphics
void InitGraphics() {
    // Shape VAO
    glGenVertexArrays(1, &shapeVAO);
    glGenBuffers(1, &shapeVBO);
    glGenBuffers(1, &shapeEBO);

    // Text VAO
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);

    CreateShaderProgram();
}

// Create the window
void MakeWindow(int width, int height, const char* title) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "ERROR: Failed to initialize GLFW" << std::endl;
        return;
    }

    // OpenGL Core Profile settings (modern OpenGL)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "ERROR: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    // Load OpenGL functions using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "ERROR: Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return;
    }

    // Set viewport size
    glViewport(0, 0, width, height);

    // Enable blending (for text transparency)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Handle window resizing
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int w, int h) {
        glViewport(0, 0, w, h);
        });

    // Initialize shaders and projection matrix for text
    InitGraphics(); // This sets up VAO/VBO and compiles shaders

    // Set up orthographic projection for text rendering
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int w, int h) {
        glViewport(0, 0, w, h);
        glm::mat4 projection = glm::ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f);
        glUseProgram(textShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(textShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        });
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

    glBindVertexArray(shapeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, shapeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(shaderProgram);
    glUniform4f(glGetUniformLocation(shaderProgram, "uColor"), r, g, b, a);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
}

// Draw a square
void DrawRectangle(float x, float y, float width, float height, float r, float g, float b, float a) {
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

    glBindVertexArray(shapeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, shapeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapeVBO);
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


bool LoadFont(Font& font, const char* fontPath, float fontSize) {
    std::ifstream fontFile(fontPath, std::ios::binary | std::ios::ate);
    if (!fontFile) {
        std::cerr << "Failed to open font file: " << fontPath << "\n";
        return false;
    }

    size_t fileSize = fontFile.tellg();
    fontFile.seekg(0, std::ios::beg);
    font.fontBuffer.resize(fileSize);
    fontFile.read(reinterpret_cast<char*>(font.fontBuffer.data()), fileSize);

    if (!stbtt_InitFont(&font.info, font.fontBuffer.data(), stbtt_GetFontOffsetForIndex(font.fontBuffer.data(), 0))) {
        std::cerr << "Failed to initialize font!\n";
        return false;
    }

    font.bitmap.resize(FONT_BITMAP_WIDTH * FONT_BITMAP_HEIGHT, 0);
    int x = 0, y = 0, maxHeight = 0;
    float scale = stbtt_ScaleForPixelHeight(&font.info, fontSize);

    font.chars.resize(128);

    for (char c = 32; c < 127; c++) {
        int w, h, xoff, yoff;
        unsigned char* charBitmap = stbtt_GetCodepointBitmap(&font.info, 0, scale, c, &w, &h, &xoff, &yoff);

        if (x + w >= FONT_BITMAP_WIDTH) {
            x = 0;
            y += maxHeight + 5;
            maxHeight = 0;
        }

        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                font.bitmap[(y + j) * FONT_BITMAP_WIDTH + (x + i)] = charBitmap[j * w + i];
            }
        }

        int advance;
        stbtt_GetCodepointHMetrics(&font.info, c, &advance, nullptr);
        font.chars[c] = { (float)advance * scale, (float)xoff, (float)yoff, (float)x, (float)y, (float)(x + w), (float)(y + h) };
        x += w + 2;
        if (h > maxHeight) maxHeight = h;

        stbtt_FreeBitmap(charBitmap, nullptr);
    }

    glGenTextures(1, &font.textureID);
    glBindTexture(GL_TEXTURE_2D, font.textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, FONT_BITMAP_WIDTH, FONT_BITMAP_HEIGHT, 0, GL_ALPHA, GL_UNSIGNED_BYTE, font.bitmap.data());

    // Create VAO and VBO for text rendering
    glGenVertexArrays(1, &font.vao);
    glGenBuffers(1, &font.vbo);
    glGenBuffers(1, &font.ebo);

    return true;
}


void DrawText(Font& font, const std::string& text, float x, float y) {
    glEnable(GL_TEXTURE_2D);  // Enable textures
    glBindTexture(GL_TEXTURE_2D, font.textureID);

    // Prepare the vertex data for the text
    std::vector<float> vertices;

    float xpos = x;
    for (char c : text) {
        if (c < 32 || c >= 127) continue;
        FontChar& fc = font.chars[c];

        float x0 = xpos + fc.offsetX;
        float y0 = y + fc.offsetY;
        float x1 = x0 + (fc.x1 - fc.x0);
        float y1 = y0 + (fc.y1 - fc.y0);

        // Add vertices for the quad
        vertices.push_back(x0); vertices.push_back(y0);  // Bottom-left
        vertices.push_back(fc.x0 / 512.0f); vertices.push_back(fc.y0 / 512.0f);  // Texture coords

        vertices.push_back(x1); vertices.push_back(y0);  // Bottom-right
        vertices.push_back(fc.x1 / 512.0f); vertices.push_back(fc.y0 / 512.0f);

        vertices.push_back(x1); vertices.push_back(y1);  // Top-right
        vertices.push_back(fc.x1 / 512.0f); vertices.push_back(fc.y1 / 512.0f);

        vertices.push_back(x0); vertices.push_back(y1);  // Top-left
        vertices.push_back(fc.x0 / 512.0f); vertices.push_back(fc.y1 / 512.0f);

        xpos += fc.advanceX;
    }

    // Create the buffer data for the current frame
    glBindVertexArray(font.vao);
    glBindBuffer(GL_ARRAY_BUFFER, font.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);

    // Set up the vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Draw the text
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, font.ebo);
    glDrawArrays(GL_QUADS, 0, vertices.size() / 4);  // 4 vertices per quad

    // Disable after drawing
    glDisable(GL_TEXTURE_2D);
}







