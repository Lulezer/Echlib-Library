#include "echlib.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <unordered_map>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>
#include <fstream>
#include <stb_truetype/stb_truetype.h>
#include <array>
#include <cstdlib> // For malloc/free or new/delete


namespace ech {
    GLFWwindow* window = nullptr;
    unsigned int vao, vbo, ebo;
    unsigned int shaderProgramShape;
    unsigned int shaderProgramTexture;
    unsigned int shaderProgramText;

    GLuint defaultFont;

    std::unordered_map<std::string, Font> fonts;  // This should be defined once


    unsigned int textureID;
    std::unordered_map<std::string, GLuint> textures;

    int targetFps;

    using namespace std::chrono;

    std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();

    int ech::windowWidth = 0;
    int ech::windowHeight = 0;

    float transparency = 1.0f;

    std::unordered_map<int, bool> mouseButtonPreviousStates;




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

uniform mat4 projection; // Projection matrix for 2D rendering

void main() {
    gl_Position = projection * vec4(aPos, 0.0, 1.0); // Apply projection matrix
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

    // Vertex Shader Source for Text Rendering
    const char* textVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);
    TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y); // Flip texture
}
)";

    // Fragment Shader Source for Text Rendering
    const char* textFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D text; // Texture for text rendering

void main()
{
    FragColor = texture(text, TexCoord);
}
)";





    // Compile shader
    void CompileShader(unsigned int shader, const char* source, const std::string& shaderType) {
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "ERROR: " << shaderType << " Shader Compilation Failed\n" << infoLog << std::endl;
        }
    }


    // Create text shader program
    void CreateTextShaderProgram() {
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        CompileShader(vertexShader, textVertexShaderSource, "Vertex");

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        CompileShader(fragmentShader, textFragmentShaderSource, "Fragment");

        shaderProgramText = glCreateProgram();
        glAttachShader(shaderProgramText, vertexShader);
        glAttachShader(shaderProgramText, fragmentShader);
        glLinkProgram(shaderProgramText);

        int success;
        char infoLog[512];
        glGetProgramiv(shaderProgramText, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgramText, 512, nullptr, infoLog);
            std::cerr << "ERROR: Text Shader Program Linking Failed\n" << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }



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
        else {
            std::cout << "Shape Shader Program Linked Successfully!" << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

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
            std::cerr << "ERROR: Shape Shader Program Linking Failed\n" << infoLog << std::endl;
        }
        else {
            std::cout << "Shape Shader Program Linked Successfully!" << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }





    // Initialize OpenGL
    void InitGraphics() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glBindVertexArray(vao);

        float vertices[] = {
            -0.5f, -0.5f, 0.0f, 0.0f,
             0.5f, -0.5f, 1.0f, 0.0f,
             0.0f,  0.5f, 0.5f, 1.0f
        };

        unsigned int indices[] = { 0, 1, 2 };

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);

        CreateTextShaderProgram();
        CreateShapeShaderProgram();
        CreateTextureShaderProgram();
    }

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

        windowWidth = width;
        windowHeight = height;
        glViewport(0, 0, width, height);

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int w, int h) {
            ech::windowWidth = w;
            ech::windowHeight = h;
            glViewport(0, 0, w, h);
            });

        InitGraphics();
    }

    void ech::SetTargetFps(int target) {
        targetFps = target;
    }

    void ech::CloseWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

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

        float flippedY = windowHeight - y;  // Flip Y to match OpenGL coordinates

        // Define vertices consistently with DrawTexturedRectangle
        float vertices[] = {
            (x / windowWidth) * 2.0f - 1.0f, 1.0f - (flippedY / windowHeight) * 2.0f,         // Bottom-left
            ((x + width) / windowWidth) * 2.0f - 1.0f, 1.0f - (flippedY / windowHeight) * 2.0f, // Bottom-right
            ((x + width) / windowWidth) * 2.0f - 1.0f, 1.0f - ((flippedY - height) / windowHeight) * 2.0f, // Top-right
            (x / windowWidth) * 2.0f - 1.0f, 1.0f - ((flippedY - height) / windowHeight) * 2.0f  // Top-left
        };

        unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };  // Two triangles for the rectangle

        // Set color uniform
        glUniform4f(glGetUniformLocation(shaderProgramShape, "uColor"), color.r, color.g, color.b, color.a);

        // Bind buffers
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);  // Draw the rectangle

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


    void ech::DrawProCircle(float centerX, float centerY, float radius, const Color& color, int segments, float transperency = 1.0f) {
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
        glUniform4f(glGetUniformLocation(shaderProgramShape, "uColor"), color.r, color.g, color.b, transperency);

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

    void ech::DrawProTriangle(float x, float y, float width, float height, const Color& color, float transparency) {
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
        glUniform4f(glGetUniformLocation(shaderProgramShape, "uColor"), color.r, color.g, color.b, transparency);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);
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

        float vertices[] = {
            (x / windowWidth) * 2.0f - 1.0f, 1.0f - (flippedY / windowHeight) * 2.0f, 0.0f, 0.0f, // Bottom-left
            ((x + width) / windowWidth) * 2.0f - 1.0f, 1.0f - (flippedY / windowHeight) * 2.0f, 1.0f, 0.0f, // Bottom-right
            ((x + width) / windowWidth) * 2.0f - 1.0f, 1.0f - ((flippedY - height) / windowHeight) * 2.0f, 1.0f, 1.0f, // Top-right
            (x / windowWidth) * 2.0f - 1.0f, 1.0f - ((flippedY - height) / windowHeight) * 2.0f, 0.0f, 1.0f  // Top-left
        };


        unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

        if (textures.find(name) == textures.end()) {
            printf("Texture not found: %s\n", name.c_str());
            return;
        }

        stbi_set_flip_vertically_on_load(true);

        
        glUseProgram(shaderProgramText);  // Activate texture shader

        // ✅ Set full opacity for non-Pro textures
        int alphaLocation = glGetUniformLocation(shaderProgramText, "alpha");
        glUniform1f(alphaLocation, 1.0f);

        glBindTexture(GL_TEXTURE_2D, textures[name]);

        // Buffer and configure VAO, VBO, and EBO
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);  // Draw the rectangle

        // Clean up
        glBindVertexArray(0);
    }


    void DrawProTexturedRectangle(float x, float y, float width, float height, float rotation, float alpha, const std::string& name) {
        if (textures.find(name) == textures.end()) {
            printf("Texture not found: %s\n", name.c_str());
            return;
        }

        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

        float halfWidth = width / 2.0f;
        float halfHeight = height / 2.0f;

        // Calculate rotated vertices
        float cosTheta = cos(rotation);
        float sinTheta = sin(rotation);

        float vertices[] = {
            -halfWidth * cosTheta - (-halfHeight) * sinTheta, -halfWidth * sinTheta + (-halfHeight) * cosTheta, 0.0f, 1.0f, // Top-left
             halfWidth * cosTheta - (-halfHeight) * sinTheta,  halfWidth * sinTheta + (-halfHeight) * cosTheta, 1.0f, 1.0f, // Top-right
             halfWidth * cosTheta - halfHeight * sinTheta,     halfWidth * sinTheta + halfHeight * cosTheta,    1.0f, 0.0f, // Bottom-right
            -halfWidth * cosTheta - halfHeight * sinTheta,    -halfWidth * sinTheta + halfHeight * cosTheta,   0.0f, 0.0f  // Bottom-left
        };

        unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

        // Move vertices to correct screen position
        for (int i = 0; i < 4; i++) {
            vertices[i * 4] = (vertices[i * 4] + x) / windowWidth * 2.0f - 1.0f;
            vertices[i * 4 + 1] = 1.0f - ((vertices[i * 4 + 1] + y) / windowHeight * 2.0f);
        }

        glBindTexture(GL_TEXTURE_2D, textures[name]);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glUseProgram(shaderProgramText);

        // Pass transparency to shader
        int alphaLocation = glGetUniformLocation(shaderProgramText, "alpha");
        glUniform1f(alphaLocation, alpha);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
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


    // Function to check if a mouse button was just pressed
    int ech::IsMouseButtonPressed(int button) {
        // Initialize the button state if it doesn't exist in the map
        if (mouseButtonPreviousStates.find(button) == mouseButtonPreviousStates.end()) {
            mouseButtonPreviousStates[button] = false;  // Set the initial state to not pressed
        }

        // Get the current state of the mouse button
        bool currentState = glfwGetMouseButton(window, button) == GLFW_PRESS;

        // Get the previous state of the mouse button
        bool previousState = mouseButtonPreviousStates[button];

        // Update the button state in the map
        mouseButtonPreviousStates[button] = currentState;

        // Return true if the mouse button was just pressed (not pressed last frame, pressed now)
        return currentState && !previousState;
    }

    // Function to detect if a mouse button is being held down (continuously pressed)
    int ech::IsMouseButtonHeld(int button) {
        // Initialize the button state if it doesn't exist in the map
        if (mouseButtonPreviousStates.find(button) == mouseButtonPreviousStates.end()) {
            mouseButtonPreviousStates[button] = false;  // Set the initial state to not pressed
        }

        // Return whether the mouse button is currently pressed
        return glfwGetMouseButton(window, button) == GLFW_PRESS;
    }

    float ech::GetDeltaTime() {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> duration = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - lastTime);
        lastTime = currentTime;
        return duration.count();
    }


    // Function to load font with stb_truetype
    bool LoadFont(const char* fontFile, int fontSize, Font& outFont) {
        // Open the font file
        FILE* fontFilePointer = fopen(fontFile, "rb");
        if (!fontFilePointer) {
            std::cerr << "Failed to open font file!" << std::endl;
            return false;
        }

        // Read the font file into a buffer
        fseek(fontFilePointer, 0, SEEK_END);
        long length = ftell(fontFilePointer);
        fseek(fontFilePointer, 0, SEEK_SET);
        unsigned char* ttfBuffer = (unsigned char*)malloc(length);
        fread(ttfBuffer, 1, length, fontFilePointer);
        fclose(fontFilePointer);

        // Initialize font using stb_truetype
        stbtt_fontinfo fontInfo;
        if (!stbtt_InitFont(&fontInfo, ttfBuffer, stbtt_GetFontOffsetForIndex(ttfBuffer, 0))) {
            std::cerr << "Failed to initialize font!" << std::endl;
            free(ttfBuffer);
            return false;
        }

        // Generate OpenGL texture
        glGenTextures(1, &outFont.textureID);
        glBindTexture(GL_TEXTURE_2D, outFont.textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        unsigned char* bitmap = new unsigned char[FONT_BITMAP_WIDTH * FONT_BITMAP_HEIGHT](); // Initialize bitmap

        // Bake the font into the bitmap (character range 32 to 96)
        int numCharsBaked = stbtt_BakeFontBitmap(ttfBuffer, 0, fontSize, bitmap,
            FONT_BITMAP_WIDTH, FONT_BITMAP_HEIGHT,
            32, 96, outFont.characterData);

        if (numCharsBaked != 256) {
            std::cerr << "Error: Not all characters were baked successfully! Only " << numCharsBaked << " characters baked." << std::endl;
            delete[] bitmap;
            free(ttfBuffer);
            return false;
        }

        // Upload bitmap to OpenGL texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, FONT_BITMAP_WIDTH, FONT_BITMAP_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);

        outFont.textureWidth = FONT_BITMAP_WIDTH;
        outFont.textureHeight = FONT_BITMAP_HEIGHT;

        delete[] bitmap;
        free(ttfBuffer);

        // Validate baked character data
        bool invalidCharFound = false;
        for (int i = 0; i < 256; ++i) {
            const stbtt_bakedchar& c = outFont.characterData[i];
            if (c.x0 == c.x1 || c.y0 == c.y1) {
                std::cerr << "Skipping invalid character at index " << i << ": "
                    << "x0=" << c.x0 << ", x1=" << c.x1 << ", "
                    << "y0=" << c.y0 << ", y1=" << c.y1 << std::endl;
                invalidCharFound = true;
            }
        }

        if (invalidCharFound) {
            std::cerr << "Warning: Some characters were invalid during baking!" << std::endl;
        }

        return true;
    }






    void DrawText(Font& font, const char* text, float x, float y, int fontSize, Color color) {
        if (!text) {
            std::cerr << "Error: text is null!" << std::endl;
            return;
        }

        if (font.textureID == 0) {
            std::cerr << "Error: Font texture not loaded!" << std::endl;
            return;
        }

        if (shaderProgramText == 0) {
            std::cerr << "Error: Shader program not initialized!" << std::endl;
            return;
        }

        glUseProgram(shaderProgramText);

        GLint textColorLocation = glGetUniformLocation(shaderProgramText, "textColor");
        glUniform4f(textColorLocation, color.r, color.g, color.b, color.a);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, font.textureID);

        static GLuint VAO = 0, VBO = 0;
        if (VAO == 0) {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
        }

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        float xPos = x, yPos = y;

        // Loop through each character in the text string
        for (const char* p = text; *p; ++p) {
            if (*p == 32) { // Skip space manually (ASCII 32)
                xPos += fontSize; // Add spacing for the space character
                continue;
            }

            if (*p < 32 || *p >= 127) continue;  // Skip characters outside the baked range (32-126)

            stbtt_bakedchar& c = font.characterData[*p - 32]; // Access the baked character

            // If character data is invalid (zero width or height), skip it
            if (c.x0 == c.x1 || c.y0 == c.y1) {
                std::cerr << "Skipping invalid character: " << *p << std::endl;
                continue;
            }

            // Calculate the position and size for the character quad
            float x2 = xPos + c.xoff;
            float y2 = yPos - c.yoff;
            float w = c.x1 - c.x0;
            float h = c.y1 - c.y0;

            // Create the vertices for the quad
            float vertices[6][4] = {
                { x2,     y2 + h,   c.x0 / font.textureWidth, c.y1 / font.textureHeight },
                { x2,     y2,       c.x0 / font.textureWidth, c.y0 / font.textureHeight },
                { x2 + w, y2,       c.x1 / font.textureWidth, c.y0 / font.textureHeight },
                { x2,     y2 + h,   c.x0 / font.textureWidth, c.y1 / font.textureHeight },
                { x2 + w, y2,       c.x1 / font.textureWidth, c.y0 / font.textureHeight },
                { x2 + w, y2 + h,   c.x1 / font.textureWidth, c.y1 / font.textureHeight }
            };

            // Update the vertex buffer with the new data
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

            // Draw the character as a quad
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Advance to the next character's position
            xPos += c.xadvance;
        }


        glBindVertexArray(0);
    }







    void GetMousePosition(double& x, double& y) {
        glfwGetCursorPos(window, &x, &y);
    }

    bool ech::CollisionShape::CheckCollision(const CollisionShape& other) {
        return (x < other.x + other.width &&
            x + width > other.x &&
            y < other.y + other.height &&
            y + height > other.y);
    }

    void DrawRectangleCollisionShape(float x, float y, float width, float height, const Color& color) {
        glUseProgram(shaderProgramShape);  // Use shape shader

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

        glUniform4f(glGetUniformLocation(shaderProgramShape, "uColor"), color.r, color.g, color.b, color.a);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }


}
