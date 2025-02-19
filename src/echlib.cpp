#include "echlib.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>

namespace ech {
    GLFWwindow* window = nullptr;
    unsigned int vao, vbo, ebo;
    unsigned int shaderProgramShape;
    unsigned int shaderProgramTexture;
    unsigned int textureID;
    std::unordered_map<std::string, GLuint> textures;

    int targetFps;

    using namespace std::chrono;

    std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();

    int ech::windowWidth = 0;
    int ech::windowHeight = 0;

    float transparency = 1.0f;


    // Shape Vertex Shader
    static const char* shapeVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0); // Directly using the vertex position
}
)";


    // Shape Fragment Shader
    static const char* shapeFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec4 uColor;

void main() {
    FragColor = uColor;
}
)";


    // Texture Vertex Shader
    static const char* textureVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0); // Directly using the vertex position
    TexCoord = aTexCoord;
}
)";


    // Texture Fragment Shader
    static const char* textureFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture1;

void main() {
    vec4 texColor = texture(texture1, TexCoord);
    if (texColor.a < 0.1) discard; // Avoid rendering fully transparent pixels
    FragColor = texColor;
}
)";







    // Shader Compilation Helper Function
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

    // Shader Program Creation for Shapes (Solid Color)
    void CreateShapeShaderProgram() {
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        CompileShader(vertexShader, shapeVertexShaderSource, "Vertex");

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        CompileShader(fragmentShader, shapeFragmentShaderSource, "Fragment");

        shaderProgramShape = glCreateProgram();
        glAttachShader(shaderProgramShape, vertexShader);
        glAttachShader(shaderProgramShape, fragmentShader);
        glLinkProgram(shaderProgramShape);

        int success;
        char infoLog[512];
        glGetProgramiv(shaderProgramShape, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgramShape, 512, nullptr, infoLog);
            std::cerr << "ERROR: Shape Shader Program Linking Failed\n" << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    // Shader Program Creation for Textures
    void CreateTextureShaderProgram() {
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        CompileShader(vertexShader, textureVertexShaderSource, "Vertex");

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        CompileShader(fragmentShader, textureFragmentShaderSource, "Fragment");

        shaderProgramTexture = glCreateProgram();
        glAttachShader(shaderProgramTexture, vertexShader);
        glAttachShader(shaderProgramTexture, fragmentShader);
        glLinkProgram(shaderProgramTexture);

        int success;
        char infoLog[512];
        glGetProgramiv(shaderProgramTexture, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgramTexture, 512, nullptr, infoLog);
            std::cerr << "ERROR: Texture Shader Program Linking Failed\n" << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }



    // Initialize Graphics (Minor Improvements)
    void InitGraphics() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDisable(GL_DEPTH_TEST);

        // Create shader programs for shapes and textures
        CreateShapeShaderProgram();
        CreateTextureShaderProgram();
    }

    // Create the Window (No Change)
    void ech::MakeWindow(int width, int height, const char* title) {
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

        // Store the window size for scaling
        windowWidth = width;
        windowHeight = height;

        // Set the initial viewport size
        glViewport(0, 0, width, height);

        // Set the callback for resizing the window
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int w, int h) {
            ech::windowWidth = w;
            ech::windowHeight = h;
            glViewport(0, 0, w, h);
            });

        // Initialize graphics (shaders, buffers, etc.)
        InitGraphics();
    }


    void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
        ech::windowWidth = width;
        ech::windowHeight = height;
        glViewport(0, 0, width, height);

        // Update projection matrix (if you're using one)
        float aspectRatio = (float)width / (float)height;
    }



    void ech::SetTargetFps(int target) {
        targetFps = target;
    }

    // Close the window
    void ech::CloseWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    // Check if the window should close
    int ech::WindowShouldClose() {
        return glfwWindowShouldClose(window);
    }

    // Start drawing
    void ech::StartDrawing() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }


    // End drawing
    void ech::EndDrawing() {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clear the background with a color
    void ech::ClearBackground(Color color) {
        glClearColor(color.r, color.g, color.b, color.a);
    }


    void ech::DrawTriangle(float x, float y, float width, float height, const Color& color) {
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

        glUseProgram(shaderProgramShape);
        glUniform4f(glGetUniformLocation(shaderProgramShape, "uColor"), color.r, color.g, color.b, color.a);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);
    }

    void ech::DrawRectangle(float x, float y, float width, float height, const Color& color) {
        glUseProgram(shaderProgramShape);  // Use the shader for solid color shapes
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

        float normalizedX = (x / (float)windowWidth) * 2.0f - 1.0f;
        float normalizedY = (y / (float)windowHeight) * 2.0f - 1.0f;
        float normalizedWidth = (width / (float)windowWidth) * 2.0f;
        float normalizedHeight = (height / (float)windowHeight) * 2.0f;

        // Set the color uniform
        glUniform4f(glGetUniformLocation(shaderProgramShape, "uColor"), color.r, color.g, color.b, color.a);

        // Define vertices for the rectangle
        float vertices[] = {
            (x / windowWidth) * 2.0f - 1.0f, 1.0f - (y / windowHeight) * 2.0f,
            ((x + width) / windowWidth) * 2.0f - 1.0f, 1.0f - (y / windowHeight) * 2.0f,
            ((x + width) / windowWidth) * 2.0f - 1.0f, 1.0f - ((y + height) / windowHeight) * 2.0f,
            (x / windowWidth) * 2.0f - 1.0f, 1.0f - ((y + height) / windowHeight) * 2.0f
        };

        unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };  // Rectangle is made of two triangles

        // Bind buffers
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

        // Vertex attributes
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Draw the rectangle
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);  // Unbind VAO
    }
    void DrawProRectangle(float x, float y, float width, float height, const Color& color, float angle, float transperency = 1.0f) {
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);


        float normalizedX = (x / (float)windowWidth) * 2.0f - 1.0f;
        float normalizedY = (y / (float)windowHeight) * 2.0f - 1.0f;
        float normalizedWidth = (width / (float)windowWidth) * 2.0f;
        float normalizedHeight = (height / (float)windowHeight) * 2.0f;

        // Flip the Y coordinate to match OpenGL's coordinate system
        float flippedY = windowHeight - y;

        // Define the rectangle's original vertices (without rotation)
        float vertices[] = {
            (x / windowWidth) * 2.0f - 1.0f, 1.0f - (flippedY / windowHeight) * 2.0f,
            ((x + width) / windowWidth) * 2.0f - 1.0f, 1.0f - (flippedY / windowHeight) * 2.0f,
            ((x + width) / windowWidth) * 2.0f - 1.0f, 1.0f - ((flippedY - height) / windowHeight) * 2.0f,
            (x / windowWidth) * 2.0f - 1.0f, 1.0f - ((flippedY - height) / windowHeight) * 2.0f
        };

        // Calculate the center of the rectangle for rotation
        float centerX = x + width / 2.0f;
        float centerY = y + height / 2.0f;

        // Normalize the center to NDC (OpenGL coordinates)
        centerX = (centerX / windowWidth) * 2.0f - 1.0f;
        centerY = 1.0f - ((windowHeight - centerY) / windowHeight) * 2.0f;

        // Apply the rotation and translation to each vertex
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

        for (int i = 0; i < 8; i += 2) {
            // Translate the vertex to the origin (subtract center), apply rotation, then translate back
            glm::vec4 vertex(vertices[i], vertices[i + 1], 0.0f, 1.0f);

            // Translate vertex to origin
            vertex.x -= centerX;
            vertex.y -= centerY;

            // Apply rotation
            vertex = rotation * vertex;

            // Translate vertex back to original position
            vertex.x += centerX;
            vertex.y += centerY;

            // Store the transformed vertices
            vertices[i] = vertex.x;
            vertices[i + 1] = vertex.y;

            // Debugging: Print the transformed vertices
            std::cout << "Transformed vertex " << i / 2 << ": (" << vertices[i] << ", " << vertices[i + 1] << ")" << std::endl;
        }

        // Use the color shader
        glUseProgram(shaderProgramShape);
        glUniform4f(glGetUniformLocation(shaderProgramShape, "uColor"), color.r, color.g, color.b, transperency);

        // Bind buffers and draw the rotated rectangle
        unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0); // Unbind VAO
    }

    void ech::DrawCircle(float centerX, float centerY, float radius, const Color& color, int segments) {
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

        float aspectRatio = (float)windowWidth / (float)windowHeight;

        // Normalize positions
        float normalizedX = (centerX / (float)windowWidth) * 2.0f - 1.0f;
        float normalizedY = (centerY / (float)windowHeight) * 2.0f - 1.0f;

        // Adjust X-axis for aspect ratio
        normalizedX *= aspectRatio;

        // Flip the Y coordinate to match OpenGL's coordinate system
        float flippedY = windowHeight - centerY;

        // Flip the Y coordinate to match OpenGL's coordinate system
        segments = 36;

        // Calculate the vertices for the circle using polar coordinates
        std::vector<float> vertices;
        for (int i = 0; i < segments; ++i) {
            float theta = (i / float(segments)) * 2.0f * glm::pi<float>();
            float x = centerX + radius * cos(theta);
            float y = flippedY - radius * sin(theta); // Flip y-axis for OpenGL coordinates
            vertices.push_back((x / windowWidth) * 2.0f - 1.0f); // Convert to NDC
            vertices.push_back(1.0f - (y / windowHeight) * 2.0f); // Convert to NDC
        }

        // Use the color shader
        glUseProgram(shaderProgramShape);
        glUniform4f(glGetUniformLocation(shaderProgramShape, "uColor"), color.r, color.g, color.b, color.a);

        // Create an index array for the circle (fan)
        std::vector<unsigned int> indices;
        for (int i = 1; i < segments - 1; ++i) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }

        // Set up the vertex data
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0); // Unbind VAO
    }

    // Texture System

    void LoadTexture(const char* filepath, const std::string& name) {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


        int width, height, nrChannels;
        unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, nrChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            textures[name] = textureID;  // Store the texture with the name in the map
            std::cout << "Texture loaded: " << name << std::endl;
        }
        else {
            std::cerr << "ERROR: Failed to load texture: " << filepath << std::endl;
        }
        stbi_image_free(data);
    }


    void DrawTexturedRectangle(float x, float y, float width, float height, const std::string& name) {
        // Retrieve window dimensions
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        float flippedY = windowHeight - y;

        float normalizedX = (x / (float)windowWidth) * 2.0f - 1.0f;
        float normalizedY = (y / (float)windowHeight) * 2.0f - 1.0f;
        float normalizedWidth = (width / (float)windowWidth) * 2.0f;
        float normalizedHeight = (height / (float)windowHeight) * 2.0f;

        // Define vertices and texture coordinates
        float vertices[] = {
            (x / windowWidth) * 2.0f - 1.0f, 1.0f - (flippedY / windowHeight) * 2.0f, 0.0f, 1.0f,
            ((x + width) / windowWidth) * 2.0f - 1.0f, 1.0f - (flippedY / windowHeight) * 2.0f, 1.0f, 1.0f,
            ((x + width) / windowWidth) * 2.0f - 1.0f, 1.0f - ((flippedY - height) / windowHeight) * 2.0f, 1.0f, 0.0f,
            (x / windowWidth) * 2.0f - 1.0f, 1.0f - ((flippedY - height) / windowHeight) * 2.0f, 0.0f, 0.0f
        };

        unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

        if (textures.find(name) == textures.end()) {
            printf("Texture not found: %s\n", name.c_str());
            return;
        }

        glBindTexture(GL_TEXTURE_2D, textures[name]);

        // Buffer and configure the VAO, VBO, and EBO for drawing
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glUseProgram(shaderProgramTexture);  // Ensure the shader program is active
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);  // Draw the textured rectangle

        // Clean up
        glBindVertexArray(0);  // Unbind the VAO
    }


    // Input System

    std::unordered_map<int, bool> keyPreviousStates;

    int ech::IsKeyPressed(int key) {
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

        // Return true if the key was Zjust pressed (not pressed last frame, pressed now)
        return currentState && !previousState;
    }

    // Detect if a key is being held down (continuously pressed)
    int ech::IsKeyHeld(int key) {
        // Initialize the key state if it doesn't exist in the map
        if (keyPreviousStates.find(key) == keyPreviousStates.end()) {
            keyPreviousStates[key] = false;  // Set the initial state to not pressed
        }

        return glfwGetKey(window, key) == GLFW_PRESS;
    }

    float GetDeltaTime() {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> duration = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - lastTime);
        lastTime = currentTime;
        return duration.count();
    }

}
