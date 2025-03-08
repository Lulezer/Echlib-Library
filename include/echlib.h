#ifndef ECHLIB_H
#define ECHLIB_H

#include <GLFW/glfw3.h>
#include <raudio.h>
#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include <vector>
#include <array>
#include <iostream>
#include "stb_truetype/stb_truetype.h"
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <ostream>


#define FONT_BITMAP_WIDTH 512
#define FONT_BITMAP_HEIGHT 512


namespace ech {

    extern int windowWidth;
    extern int windowHeight;
    extern float transparency;

    // Define a Color struct
    struct Color {
        float r, g, b, a;
    };

    extern std::unordered_map<std::string, GLuint> textures;

    enum class TextureType {
        NEAREST = GL_NEAREST,
        LINEAR = GL_LINEAR
    };



    // Define common colors
    const Color WHITE = { 1.0f, 1.0f, 1.0f, transparency };   // Full white
    const Color BLACK = { 0.0f, 0.0f, 0.0f, transparency };   // Full black
    const Color RED = { 1.0f, 0.0f, 0.0f, transparency };     // Red
    const Color GRAY = { 0.5f, 0.5f, 0.5f, transparency };    // Gray
    const Color YELLOW = { 1.0f, 1.0f, 0.0f, transparency };  // Yellow
    const Color CYAN = { 0.0f, 1.0f, 1.0f, transparency };    // Cyan
    const Color MAGENTA = { 1.0f, 0.0f, 1.0f, transparency }; // Magenta
    const Color ORANGE = { 1.0f, 0.647f, 0.0f, transparency }; // Orange
    const Color PURPLE = { 0.5f, 0.0f, 0.5f, transparency };  // Purple
    const Color PINK = { 1.0f, 0.75f, 0.796f, transparency }; // Pink
    const Color BROWN = { 0.545f, 0.298f, 0.149f, transparency }; // Brown
    const Color LIGHT_BLUE = { 0.678f, 0.847f, 0.902f, transparency }; // Light Blue
    const Color BEIGE = { 0.827f, 0.690f, 0.514f, transparency }; // Beige
    const Color LIGHT_GREEN = { 0.565f, 0.933f, 0.565f, transparency }; // Light Green
    const Color DARK_GREEN = { 0.0f, 0.459f, 0.173f, transparency };  // Dark Green
    const Color LIGHT_CORAL = { 0.941f, 0.502f, 0.502f, transparency }; // Light Coral

    // Keys (key codes)
#define KEY_UNKNOWN GLFW_KEY_UNKNOWN
#define KEY_SPACE GLFW_KEY_SPACE
#define KEY_APOSTROPHE GLFW_KEY_APOSTROPHE
#define KEY_COMMA GLFW_KEY_COMMA
#define KEY_MINUS GLFW_KEY_MINUS
#define KEY_PERIOD GLFW_KEY_PERIOD
#define KEY_SLASH GLFW_KEY_SLASH
#define KEY_0 GLFW_KEY_0
#define KEY_1 GLFW_KEY_1
#define KEY_2 GLFW_KEY_2
#define KEY_3 GLFW_KEY_3
#define KEY_4 GLFW_KEY_4
#define KEY_5 GLFW_KEY_5
#define KEY_6 GLFW_KEY_6
#define KEY_7 GLFW_KEY_7
#define KEY_8 GLFW_KEY_8
#define KEY_9 GLFW_KEY_9
#define KEY_SEMICOLON GLFW_KEY_SEMICOLON
#define KEY_EQUAL GLFW_KEY_EQUAL
#define KEY_A GLFW_KEY_A
#define KEY_B GLFW_KEY_B
#define KEY_C GLFW_KEY_C
#define KEY_D GLFW_KEY_D
#define KEY_E GLFW_KEY_E
#define KEY_F GLFW_KEY_F
#define KEY_G GLFW_KEY_G
#define KEY_H GLFW_KEY_H
#define KEY_I GLFW_KEY_I
#define KEY_J GLFW_KEY_J
#define KEY_K GLFW_KEY_K
#define KEY_L GLFW_KEY_L
#define KEY_M GLFW_KEY_M
#define KEY_N GLFW_KEY_N
#define KEY_O GLFW_KEY_O
#define KEY_P GLFW_KEY_P
#define KEY_Q GLFW_KEY_Q
#define KEY_R GLFW_KEY_R
#define KEY_S GLFW_KEY_S
#define KEY_T GLFW_KEY_T
#define KEY_U GLFW_KEY_U
#define KEY_V GLFW_KEY_V
#define KEY_W GLFW_KEY_W
#define KEY_X GLFW_KEY_X
#define KEY_Y GLFW_KEY_Y
#define KEY_Z GLFW_KEY_Z
#define KEY_LEFT_BRACKET GLFW_KEY_LEFT_BRACKET
#define KEY_BACKSLASH GLFW_KEY_BACKSLASH
#define KEY_RIGHT_BRACKET GLFW_KEY_RIGHT_BRACKET
#define KEY_GRAVE_ACCENT GLFW_KEY_GRAVE_ACCENT
#define KEY_WORLD_1 GLFW_KEY_WORLD_1
#define KEY_WORLD_2 GLFW_KEY_WORLD_2

// Function Keys
#define KEY_ESCAPE GLFW_KEY_ESCAPE
#define KEY_ENTER GLFW_KEY_ENTER
#define KEY_TAB GLFW_KEY_TAB
#define KEY_BACKSPACE GLFW_KEY_BACKSPACE
#define KEY_INSERT GLFW_KEY_INSERT
#define KEY_DELETE GLFW_KEY_DELETE
#define KEY_RIGHT GLFW_KEY_RIGHT
#define KEY_LEFT GLFW_KEY_LEFT
#define KEY_DOWN GLFW_KEY_DOWN
#define KEY_UP GLFW_KEY_UP
#define KEY_PAGE_UP GLFW_KEY_PAGE_UP
#define KEY_PAGE_DOWN GLFW_KEY_PAGE_DOWN
#define KEY_HOME GLFW_KEY_HOME
#define KEY_END GLFW_KEY_END
#define KEY_CAPS_LOCK GLFW_KEY_CAPS_LOCK
#define KEY_SCROLL_LOCK GLFW_KEY_SCROLL_LOCK
#define KEY_NUM_LOCK GLFW_KEY_NUM_LOCK
#define KEY_PRINT_SCREEN GLFW_KEY_PRINT_SCREEN
#define KEY_PAUSE GLFW_KEY_PAUSE
#define KEY_F1 GLFW_KEY_F1
#define KEY_F2 GLFW_KEY_F2
#define KEY_F3 GLFW_KEY_F3
#define KEY_F4 GLFW_KEY_F4
#define KEY_F5 GLFW_KEY_F5
#define KEY_F6 GLFW_KEY_F6
#define KEY_F7 GLFW_KEY_F7
#define KEY_F8 GLFW_KEY_F8
#define KEY_F9 GLFW_KEY_F9
#define KEY_F10 GLFW_KEY_F10
#define KEY_F11 GLFW_KEY_F11
#define KEY_F12 GLFW_KEY_F12
#define KEY_F13 GLFW_KEY_F13
#define KEY_F14 GLFW_KEY_F14
#define KEY_F15 GLFW_KEY_F15
#define KEY_F16 GLFW_KEY_F16
#define KEY_F17 GLFW_KEY_F17
#define KEY_F18 GLFW_KEY_F18
#define KEY_F19 GLFW_KEY_F19
#define KEY_F20 GLFW_KEY_F20
#define KEY_F21 GLFW_KEY_F21
#define KEY_F22 GLFW_KEY_F22
#define KEY_F23 GLFW_KEY_F23
#define KEY_F24 GLFW_KEY_F24
#define KEY_F25 GLFW_KEY_F25

// Numpad Keys
#define KEY_KP_0 GLFW_KEY_KP_0
#define KEY_KP_1 GLFW_KEY_KP_1
#define KEY_KP_2 GLFW_KEY_KP_2
#define KEY_KP_3 GLFW_KEY_KP_3
#define KEY_KP_4 GLFW_KEY_KP_4
#define KEY_KP_5 GLFW_KEY_KP_5
#define KEY_KP_6 GLFW_KEY_KP_6
#define KEY_KP_7 GLFW_KEY_KP_7
#define KEY_KP_8 GLFW_KEY_KP_8
#define KEY_KP_9 GLFW_KEY_KP_9
#define KEY_KP_DECIMAL GLFW_KEY_KP_DECIMAL
#define KEY_KP_DIVIDE GLFW_KEY_KP_DIVIDE
#define KEY_KP_MULTIPLY GLFW_KEY_KP_MULTIPLY
#define KEY_KP_SUBTRACT GLFW_KEY_KP_SUBTRACT
#define KEY_KP_ADD GLFW_KEY_KP_ADD
#define KEY_KP_ENTER GLFW_KEY_KP_ENTER
#define KEY_KP_EQUAL GLFW_KEY_KP_EQUAL

// Modifier Keys
#define KEY_LEFT_SHIFT GLFW_KEY_LEFT_SHIFT
#define KEY_LEFT_CONTROL GLFW_KEY_LEFT_CONTROL
#define KEY_LEFT_ALT GLFW_KEY_LEFT_ALT
#define KEY_LEFT_SUPER GLFW_KEY_LEFT_SUPER
#define KEY_RIGHT_SHIFT GLFW_KEY_RIGHT_SHIFT
#define KEY_RIGHT_CONTROL GLFW_KEY_RIGHT_CONTROL
#define KEY_RIGHT_ALT GLFW_KEY_RIGHT_ALT
#define KEY_RIGHT_SUPER GLFW_KEY_RIGHT_SUPER
#define KEY_MENU GLFW_KEY_MENU

// Functions
// Function declarations
    void MakeWindow(int width, int height, const char* title);
    void CloseWindow();
    int WindowShouldClose();
    void StartDrawing();
    void EndDrawing();
    void ClearBackground(Color color);
    void SetTargetFps(int targetFps);

    // Shape Rendering
    void DrawTriangle(float x, float y, float width, float height, const Color& color);
    void DrawRectangle(float x, float y, float width, float height, const Color& color);
    void DrawProRectangle(float x, float y, float width, float height, const Color& color, float angle, float transparency);
    void DrawCircle(float centerX, float centerY, float radius, const Color& color, int segments = 36);
    void DrawProCircle(float centerX, float centerY, float radius, const Color& color, int segments, float transparency);
    void DrawProTriangle(float x, float y, float width, float height, const Color& color, float transparency);
    void DrawProTexturedRectangle(float x, float y, float width, float height, float rotation, float alpha, const std::string& name);



    // Input System
    int IsKeyPressed(int key);
    int IsKeyHeld(int key);

    // Texture Rendering
    void LoadTexture(const char* filepath, const std::string& name);
    void DrawTexturedRectangle(float x, float y, float width, float height, const std::string& name);



    // Time Management
    float GetDeltaTime();


    template <typename T>
    inline void savefile(const std::string& filename, const T& data) {
        std::ofstream file(filename, std::ios::binary);
        if (file.is_open()) {
            file.write(reinterpret_cast<const char*>(&data), sizeof(T));
            file.close();
        }
        else {
            std::cerr << "Failed to open file for saving: " << filename << std::endl;
        }
    }

    template <typename T>
    inline void loadfile(const std::string& filename, T& data) {
        std::ifstream file(filename, std::ios::binary);
        if (file.is_open()) {
            file.read(reinterpret_cast<char*>(&data), sizeof(T));
            file.close();
        }
        else {
            std::cerr << "Failed to open file for loading: " << filename << std::endl;
        }
    }

    // Specialized function for std::string
    template <>
    inline void savefile<std::string>(const std::string& filename, const std::string& data) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << data;
            file.close();
        }
        else {
            std::cerr << "Failed to open file for saving string: " << filename << std::endl;
        }
    }

    template <>
    inline void loadfile<std::string>(const std::string& filename, std::string& data) {
        std::ifstream file(filename);
        if (file.is_open()) {
            std::getline(file, data, '\0'); // Read entire file
            file.close();
        }
        else {
            std::cerr << "Failed to open file for loading string: " << filename << std::endl;
        }
    }

}
#endif // ECHLIB_H
