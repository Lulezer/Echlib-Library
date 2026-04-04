#include "Echlib.hpp"
#include <iostream>
#include <string>

int main() {
    // 1. Initialize Window
    ech::CreateWindow(1200, 720, "Echlib 1.1 - The Grand Integration Test");
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
        // Start drawing and sync matrices for Shape, Texture, and Text shaders
        ech::StartDrawing();
        ech::ClearBackground(ech::GRAY);

        // --- 1. INPUT & LOGIC ---
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

        // Update Camera (Center on player with a lerp factor for smoothness)
        ech::UpdateCamera(playerX, playerY, 0.1f, 1200.0f, 720.0f);

        // --- 2. DRAWING (Order Matters!) ---

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

        // Layer 4: HUD (UI should NOT follow the camera)
        // To draw UI, we reset the View Matrix to Identity so it stays fixed on screen
        // NOTE: If your engine doesn't have a 'ResetCamera' yet, 
        // you can draw text at coordinates relative to the camera position.

        std::string status = isColliding ? "STATUS: COLLIDING!" : "STATUS: CLEAR";
        ech::DrawText(mainFont, status, 20, 20, isColliding ? ech::RED : ech::DARK_GREEN);

        std::string fps = "FPS: " + std::to_string((int)(1.0f / dt));
        ech::DrawText(mainFont, fps, 1050, 20, ech::WHITE);

        // --- 3. END FRAME ---
        ech::EndDrawing();
    }

    ech::CloseWindow();
    return 0;
}#include "Echlib.hpp"
#include <iostream>
#include <string>

int main() {
    // 1. Initialize Window
    ech::CreateWindow(1200, 720, "Echlib 1.1 - The Grand Integration Test");
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
        // Start drawing and sync matrices for Shape, Texture, and Text shaders
        ech::StartDrawing();
        ech::ClearBackground(ech::GRAY);

        // --- 1. INPUT & LOGIC ---
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

        // Update Camera (Center on player with a lerp factor for smoothness)
        ech::UpdateCamera(playerX, playerY, 0.1f, 1200.0f, 720.0f);

        // --- 2. DRAWING (Order Matters!) ---

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

        // Layer 4: HUD (UI should NOT follow the camera)
        // To draw UI, we reset the View Matrix to Identity so it stays fixed on screen
        // NOTE: If your engine doesn't have a 'ResetCamera' yet, 
        // you can draw text at coordinates relative to the camera position.

        std::string status = isColliding ? "STATUS: COLLIDING!" : "STATUS: CLEAR";
        ech::DrawText(mainFont, status, 20, 20, isColliding ? ech::RED : ech::DARK_GREEN);

        std::string fps = "FPS: " + std::to_string((int)(1.0f / dt));
        ech::DrawText(mainFont, fps, 1050, 20, ech::WHITE);

        // --- 3. END FRAME ---
        ech::EndDrawing();
    }

    ech::CloseWindow();
    return 0;
}
