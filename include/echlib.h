#pragma once
#include <cstddef>
#include <glm/glm.hpp>            // For glm::mat4, glm::vec2
#include <glm/gtc/matrix_transform.hpp> // Optional if using glm::translate/rotate/scale

namespace ech {

    struct Color {
        float r, g, b, a;
    };

    struct Vec2 {
        float x, y;
    };

    extern glm::mat4 projection;
    extern glm::mat4 view;
    extern glm::vec2 cameraPos;
    struct Camera {
        float x, y;          // Position
        float rotation;      // Rotation in degrees
        float zoom;          // Zoom level 

        Camera() : x(0), y(0), rotation(0), zoom(1.0f) {} // Constructor to initialize defaults
    };

    inline Camera camera; // Declare a global camera instance

    // Define transparency value (fully opaque)
    constexpr float transparency = 1.0f;

    // Common colors
    inline const Color WHITE = { 1.0f, 1.0f, 1.0f, transparency };
    inline const Color BLACK = { 0.0f, 0.0f, 0.0f, transparency };
    inline const Color RED = { 1.0f, 0.0f, 0.0f, transparency };
    inline const Color GRAY = { 0.5f, 0.5f, 0.5f, transparency };
    inline const Color YELLOW = { 1.0f, 1.0f, 0.0f, transparency };
    inline const Color CYAN = { 0.0f, 1.0f, 1.0f, transparency };
    inline const Color MAGENTA = { 1.0f, 0.0f, 1.0f, transparency };
    inline const Color ORANGE = { 1.0f, 0.647f, 0.0f, transparency };
    inline const Color PURPLE = { 0.5f, 0.0f, 0.5f, transparency };
    inline const Color PINK = { 1.0f, 0.75f, 0.796f, transparency };
    inline const Color BROWN = { 0.545f, 0.298f, 0.149f, transparency };
    inline const Color LIGHT_BLUE = { 0.678f, 0.847f, 0.902f, transparency };
    inline const Color BEIGE = { 0.827f, 0.690f, 0.514f, transparency };
    inline const Color LIGHT_GREEN = { 0.565f, 0.933f, 0.565f, transparency };
    inline const Color DARK_GREEN = { 0.0f, 0.459f, 0.173f, transparency };
    inline const Color LIGHT_CORAL = { 0.941f, 0.502f, 0.502f, transparency };
    inline const Color TRANSPARENT = { 0, 0, 0, 0 };

    enum Key {
        KEY_UNKNOWN = -1,
        KEY_A, KEY_B, KEY_C, KEY_D, KEY_E,
        KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
        KEY_K, KEY_L, KEY_M, KEY_N, KEY_O,
        KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
        KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
        KEY_SPACE,
        KEY_ESCAPE,
        KEY_ENTER,
        KEY_COUNT
    };

    enum MouseKeys {
        MOUSE_LEFT_BUTTON, MOUSE_RIGHT_BUTTON,
        MOUSE_MIDDLE_BUTTON
    };

    // Window / GL
    void CreateWindow(int width, int height, const char* title);
    void CloseWindow();
    int WindowShouldClose();

    // Graphics init
    void InitGraphics();

    // Frame control
    void StartDrawing();
    void EndDrawing();
    void ClearBackground(Color color);

    // Draw primitives (world coordinates)
    void DrawLine(float x1, float y1, float x2, float y2, Color color);
    void DrawTriangle(float x, float y, float w, float h, Color color);
    void DrawRectangle(float x, float y, float w, float h, Color color);
    void DrawCircle(float x, float y, float radius, Color color);
    void DrawTexturedRectangle(float x, float y, float w, float h, unsigned int textureID);

    // Texture
    unsigned int LoadTexture(const char* path);

    // Input
    int IsKeyPressed(int key);
    int IsKeyHeld(int key);
    int IsMouseButtonPressed(int button);
    int IsMouseButtonHeld(int button);

    // Camera
    void UpdateCamera(float targetX, float targetY, float lerpFactor, float screenWidth, float screenHeight);
	void ProUpdateCamera(float targetX, float targetY, float lerpFactor, float screenWidth, float screenHeight, float zoom);
    // Timing
    float GetDeltaTime();
}
