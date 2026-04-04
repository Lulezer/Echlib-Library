#include "Echlib.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <fstream>
#include <filesystem>

// stb_image for textures
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype/stb_truetype.h>

// glm for matrices
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "internal.hpp"
#include "graphics_internal.hpp"

namespace ech {

    // === GLOBALS ===
    int windowWidth = 0;
    int windowHeight = 0;



    std::unordered_map<int, bool> mouseButtonPreviousStates;
    std::unordered_map<int, bool> keyPreviousStates;

    constexpr float PI = 3.14159265359f;

    // === Timing & Globals (CLEANED) ===
    static float deltaTime = 0.0f;
    static std::chrono::high_resolution_clock::time_point lastFrameTime;
    static std::chrono::high_resolution_clock::time_point frameStart;
    static double targetFrameTime = 0.0;

    glm::vec2 cameraPos(0.0f, 0.0f);

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

    void CreateWindow(int w, int h, const char* title) {
        if (!GetDefaultWindow())
            GetDefaultWindow() = new ech::Window(w, h, title);
    }

    void CloseWindow() {
        delete GetDefaultWindow();
        GetDefaultWindow() = nullptr;
    }

    int WindowShouldClose() {
        return GetDefaultWindow() ? GetDefaultWindow()->ShouldClose() : 1;
    }

    float GetDeltaTime() {
        return deltaTime;
    }

    void SetFpsLimit(int fps) {
        if (fps <= 0) targetFrameTime = 0.0;
        else targetFrameTime = 1.0 / static_cast<double>(fps);
    }

    void StartDrawing() {
        frameStart = std::chrono::high_resolution_clock::now();
        glClear(GL_COLOR_BUFFER_BIT);

        // Sync Matrices for BOTH shaders
        unsigned int list[] = { shaderProgramShape, shaderProgramText };
        for (unsigned int s : list) {
            if (s == 0) continue;
            glUseProgram(s);
            glUniformMatrix4fv(glGetUniformLocation(s, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(glGetUniformLocation(s, "uView"), 1, GL_FALSE, glm::value_ptr(view));
        }
        glUseProgram(shaderProgramShape); // Default to shapes
    }

    void EndDrawing() {
        glfwSwapBuffers(GetDefaultWindow()->GetNativeHandle());
        glfwPollEvents();

        if (targetFrameTime > 0.0) {
            auto now = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(now - frameStart).count();

            while (elapsed < targetFrameTime) {
                if (targetFrameTime - elapsed > 0.002) {
                    std::this_thread::sleep_for(std::chrono::microseconds(500));
                }
                else {
                    std::this_thread::yield();
                }
                now = std::chrono::high_resolution_clock::now();
                elapsed = std::chrono::duration<double>(now - frameStart).count();
            }
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;
    }

    void StartDrawingAdv(Window& window)
    {
        frameStart = std::chrono::high_resolution_clock::now(); // Use high_res for consistency

        GLFWwindow* native = window.GetNativeHandle();
        glfwMakeContextCurrent(native);

        int fbw, fbh;
        glfwGetFramebufferSize(native, &fbw, &fbh);
        glViewport(0, 0, fbw, fbh);

        // Update the projection matrix to match the window size (important for multi-window)
        projection = glm::ortho(0.0f, (float)fbw, (float)fbh, 0.0f, -1.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        // Loop through all shaders to send the same matrices
        unsigned int activeShaders[] = { shaderProgramShape, shaderProgramTexture, shaderProgramText };
        for (unsigned int s : activeShaders) {
            if (s == 0) continue;
            glUseProgram(s);
            glUniformMatrix4fv(glGetUniformLocation(s, "uProjection"), 1, GL_FALSE, &projection[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(s, "uView"), 1, GL_FALSE, &view[0][0]);
        }
    }

    void EndDrawingAdv(Window& window)
    {
        glfwSwapBuffers(window.GetNativeHandle());

        auto currentTime = std::chrono::steady_clock::now();
        deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;
		glfwPollEvents();
    }




    // void BeginFrame()
    // {
    // 
    //     if (!limiterInitialized)
    //     {
    //         nextFrameTime = frameStart;
    //         limiterInitialized = true;
    //     }
    // }
     
     void PollEvents()
     {
         glfwPollEvents();
     }
     
   // void EndFrame()
   // {
   //     if (targetFrameTime <= 0.0)
   //         return;
   // 
   //     auto now = std::chrono::steady_clock::now();
   // 
   //     nextFrameTime += std::chrono::duration_cast<std::chrono::steady_clock::duration>(
   //         std::chrono::duration<double>(targetFrameTime)
   //     );
   // 
   //     if (nextFrameTime < now)
   //         nextFrameTime = now;
   // 
   //     std::this_thread::sleep_until(nextFrameTime);
   // }

     void SetVSync(bool enabled) {
         GLFWwindow* native = GetDefaultWindow()->GetNativeHandle();
         glfwMakeContextCurrent(native);
         glfwSwapInterval(enabled ? 1 : 0);
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
        if (textureID == 0) return;

        float vertices[] = {
            x,     y,     0.0f, 0.0f, // Top Left
            x + w, y,     1.0f, 0.0f, // Top Right
            x + w, y + h, 1.0f, 1.0f, // Bottom Right
            x,     y + h, 0.0f, 1.0f  // Bottom Left
        };
        unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

        glUseProgram(shaderProgramTexture);

        // Sync Matrices
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramTexture, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramTexture, "uView"), 1, GL_FALSE, glm::value_ptr(view));

        // Bind Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(glGetUniformLocation(shaderProgramTexture, "texture1"), 0);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
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
        if (!GetDefaultWindow()->GetNativeHandle()) return 0;

        int state = glfwGetKey(GetDefaultWindow()->GetNativeHandle(), TranslateKey(key)) == GLFW_PRESS;
        int wasPressed = keyPreviousStates[key];

        keyPreviousStates[key] = state; // update state

        // only return true when key *just* pressed
        return    state && !wasPressed;
    }

    int IsKeyHeld(int key) {
        if (!GetDefaultWindow()->GetNativeHandle()) return 0;
        int state = glfwGetKey(GetDefaultWindow()->GetNativeHandle(), TranslateKey(key));
        return state == GLFW_REPEAT || state == GLFW_PRESS;
    }

    int IsMouseButtonPressed(int button) {
        if (!GetDefaultWindow()->GetNativeHandle()) return 0;

        int glfwButton = TranslateMouseButton(button);
        if (glfwButton == -1) return 0;

        int state = glfwGetMouseButton(GetDefaultWindow()->GetNativeHandle(), glfwButton) == GLFW_PRESS;
        int wasPressed = mouseButtonPreviousStates[glfwButton];

        mouseButtonPreviousStates[glfwButton] = state;

        return state && !wasPressed;
    }

    int IsMouseButtonHeld(int button) {
        if (!GetDefaultWindow()->GetNativeHandle()) return 0;

        int glfwButton = TranslateMouseButton(button);
        if (glfwButton == -1) return 0;

        int state = glfwGetMouseButton(GetDefaultWindow()->GetNativeHandle(), glfwButton);
        return state == GLFW_PRESS;
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


    


    Font::Font() : ttfBuffer(nullptr), bitmap(nullptr), textureID(0), fontHeight(0) {}

    Font::Font(const std::string& path, float pixelHeight) {
        Load(path, pixelHeight);
    }

    Font::~Font() {
        delete[] ttfBuffer;
        delete[] bitmap;
        if (textureID) glDeleteTextures(1, &textureID);
    } 

    bool Font::Load(const std::string& path, float pixelHeight) {
        fontHeight = pixelHeight;
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file) return false;
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        ttfBuffer = new unsigned char[size];
        file.read((char*)ttfBuffer, size);
        file.close();

        bitmap = new unsigned char[512 * 512];
        // '1' at the end for OpenGL flipped Y
        stbtt_BakeFontBitmap(ttfBuffer, 0, pixelHeight, bitmap, 512, 512, 32, 96, cdata);

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // CRITICAL
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        return true;
    }

    void Font::Draw(const std::string& text, float x, float y, Color color) {
        if (!textureID || !shaderProgramText) return;

        std::vector<float> verts;
        float xpos = x; float ypos = y;
        for (unsigned char ch : text) {
            if (ch < 32 || ch >= 128) continue;
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(cdata, 512, 512, ch - 32, &xpos, &ypos, &q, 1);

            verts.insert(verts.end(), {
                q.x0, q.y0, q.s0, q.t0, q.x1, q.y0, q.s1, q.t0, q.x1, q.y1, q.s1, q.t1,
                q.x0, q.y0, q.s0, q.t0, q.x1, q.y1, q.s1, q.t1, q.x0, q.y1, q.s0, q.t1
                });
        }

        glUseProgram(shaderProgramText);
        glUniform3f(glGetUniformLocation(shaderProgramText, "textColor"), color.r, color.g, color.b);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(glGetUniformLocation(shaderProgramText, "textAtlas"), 0);

        glBindVertexArray(textVAO);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        // Use glBufferData to avoid 1281 errors if the string is long
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW);

        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(verts.size() / 4));
        glBindVertexArray(0);
    
}
    void DrawText(Font& font, const std::string& text, float x, float y, Color color) {
        font.Draw(text, x, y, color);
    }

    void ShutDown()
    {
        glfwTerminate();
    }


} // namespace ech
