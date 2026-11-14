// 2D camera example: a player that walks around a world bigger than the window,
// with a smooth follow camera, zoom, and a HUD that stays put on the screen.
#include <echlib.h>
#include <string>

int main()
{
    const float SCREEN_W = 800.0f;
    const float SCREEN_H = 600.0f;

    ech::CreateWindow((int)SCREEN_W, (int)SCREEN_H, "Camera example with echlib");
    ech::SetFpsLimit(60);

    // Player, in world coordinates
    float playerX = 400.0f;
    float playerY = 300.0f;
    const float playerSize = 48.0f;
    const float speed = 300.0f; // Pixels per second

    float zoom = 1.0f;

    ech::ClearBackground(ech::BEIGE);

    while (!ech::WindowShouldClose())
    {
        float dt = ech::GetDeltaTime();

        // --- Input ---
        if (ech::IsKeyHeld(ech::KEY_A)) playerX -= speed * dt;
        if (ech::IsKeyHeld(ech::KEY_D)) playerX += speed * dt;
        if (ech::IsKeyHeld(ech::KEY_W)) playerY -= speed * dt;
        if (ech::IsKeyHeld(ech::KEY_S)) playerY += speed * dt;

        if (ech::IsKeyPressed(ech::KEY_Q)) zoom -= 0.25f;
        if (ech::IsKeyPressed(ech::KEY_E)) zoom += 0.25f;
        if (zoom < 0.25f) zoom = 0.25f;

        // --- Camera ---
        // Update the camera BEFORE StartDrawing(): StartDrawing() is what uploads the
        // view matrix to the shaders, so a camera change made after it is ignored by
        // shapes and text until the next frame.
        //
        // The target is the centre of the player, so the player sits in the middle of
        // the screen. lerpFactor 1.0 snaps instantly, lower values trail behind.
        float targetX = playerX + playerSize / 2.0f;
        float targetY = playerY + playerSize / 2.0f;

        // UpdateCamera() follows without zoom; ProUpdateCamera() takes a zoom factor.
        ech::ProUpdateCamera(targetX, targetY, 0.1f, SCREEN_W, SCREEN_H, zoom);

        ech::StartDrawing();

        // --- World (moves with the camera) ---
        // A grid so the camera movement is visible
        for (int x = 0; x <= 2000; x += 100)
            ech::DrawLine((float)x, 0.0f, (float)x, 2000.0f, ech::GRAY);
        for (int y = 0; y <= 2000; y += 100)
            ech::DrawLine(0.0f, (float)y, 2000.0f, (float)y, ech::GRAY);

        // Some landmarks
        ech::DrawRectangle(900, 400, 150, 150, ech::LIGHT_BLUE);
        ech::DrawCircle(300, 1200, 80.0f, ech::PURPLE);

        // The player
        ech::DrawRectangle(playerX, playerY, playerSize, playerSize, ech::LIGHT_GREEN);

        // --- HUD (should not move with the camera) ---
        // Shapes use the camera's view matrix, so the trick is to draw the HUD at the
        // world position that maps to the screen corner: the camera's position minus
        // half the screen. (Zoom scales this, so a HUD and zoom together need care.)
        float hudX = ech::camera.x - SCREEN_W / 2.0f;
        float hudY = ech::camera.y - SCREEN_H / 2.0f;
        ech::DrawRectangle(hudX + 10, hudY + 10, 220, 60, { 0.0f, 0.0f, 0.0f, 0.5f });

        ech::EndDrawing();
    }

    ech::CloseWindow();
    ech::Shutdown();
    return 0;
}
