#include "echlib.h"
#include <iostream>
#include <string>

int main() {
    const float SCREEN_W = 1200.0f;
    const float SCREEN_H = 720.0f;

    // 1. Initialize Window
    ech::CreateWindow((int)SCREEN_W, (int)SCREEN_H, "Echlib - The Grand Integration Test");
    ech::SetFpsLimit(144);

    // 2. Load Assets (Ensure paths match your project structure)
    unsigned int pigTex = ech::LoadTexture(RESOURCES_PATH "pig.png");
    ech::Font mainFont(RESOURCES_PATH "123.ttf", 36.0f);

    // 3. Game State
    float playerX = 600.0f;
    float playerY = 360.0f;
    float playerSize = 64.0f;
    float moveSpeed = 400.0f; // Pixels per second

    float wallX = 800.0f;
    float wallY = 300.0f;
    float wallW = 100.0f;
    float wallH = 100.0f;

    bool isColliding = false;

    // 4. Main Game Loop
    while (!ech::WindowShouldClose()) {
        // --- 1. INPUT & LOGIC (before StartDrawing) ---
        float dt = ech::GetDeltaTime();

        // Basic Movement
        if (ech::IsKeyHeld(ech::KEY_W)) playerY -= moveSpeed * dt;
        if (ech::IsKeyHeld(ech::KEY_S)) playerY += moveSpeed * dt;
        if (ech::IsKeyHeld(ech::KEY_A)) playerX -= moveSpeed * dt;
        if (ech::IsKeyHeld(ech::KEY_D)) playerX += moveSpeed * dt;

        // Collision Check
        isColliding = ech::CheckCollision(
            playerX, playerY, playerSize, playerSize,
            wallX, wallY, wallW, wallH
        );

        // Center the camera on the player. StartDrawing() uploads the matrices,
        // so the camera has to be updated before it.
        ech::UpdateCamera(playerX, playerY, 0.1f, SCREEN_W, SCREEN_H);

        // --- 2. DRAWING (Order Matters!) ---
        ech::StartDrawing();
        ech::ClearBackground(ech::GRAY);

        // Layer 0: The World/Background
        ech::DrawRectangle(0, 0, 2000, 2000, ech::BLACK); // A giant floor

        // Layer 1: Obstacles
        // Change color to RED if we hit it
        ech::Color wallColor = isColliding ? ech::RED : ech::LIGHT_BLUE;
        ech::DrawRectangle(wallX, wallY, wallW, wallH, wallColor);

        // Layer 2: The Player (Texture)
        ech::DrawTexturedRectangle(playerX, playerY, playerSize, playerSize, pigTex);

        // Layer 3: Particles/Shapes
        ech::DrawCircle(playerX + (playerSize / 2), playerY + (playerSize / 2), 10.0f, ech::YELLOW);

        // Layer 4: HUD. Shapes and text use the camera's view matrix, so a HUD is
        // drawn at world coordinates that map back to a fixed screen position.
        float hudX = ech::camera.x - SCREEN_W / 2.0f;
        float hudY = ech::camera.y - SCREEN_H / 2.0f;

        std::string status = isColliding ? "STATUS: COLLIDING!" : "STATUS: CLEAR";
        ech::DrawText(mainFont, status, hudX + 20, hudY + 50, isColliding ? ech::RED : ech::DARK_GREEN);

        std::string fps = "FPS: " + std::to_string(dt > 0.0f ? (int)(1.0f / dt) : 0);
        ech::DrawText(mainFont, fps, hudX + 1050, hudY + 50, ech::WHITE);

        // --- 3. END FRAME ---
        ech::EndDrawing();
    }

    ech::CloseWindow();
    ech::Shutdown();
    return 0;
}
