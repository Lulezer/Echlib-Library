#include <echlib.h> // Include echlib for the window, input and drawing functions

int main()
{
    int windowWidth = 640;  // Width of the window in pixels
    int windowHeight = 360; // Height of the window in pixels

    float boxX = 300.0f;
    float boxY = 160.0f;
    float speed = 250.0f; // Pixels per second

    ech::CreateWindow(windowWidth, windowHeight, "Holding a key with echlib");
    ech::SetFpsLimit(60); // EndDrawing() does the waiting for you

    ech::ClearBackground(ech::WHITE);

    while (!ech::WindowShouldClose())
    {
        // IsKeyHeld() is true every frame the key is down, which is what movement wants.
        // Multiply by the delta time so the speed does not depend on the frame rate.
        float dt = ech::GetDeltaTime();

        if (ech::IsKeyHeld(ech::KEY_A)) boxX -= speed * dt;
        if (ech::IsKeyHeld(ech::KEY_D)) boxX += speed * dt;
        if (ech::IsKeyHeld(ech::KEY_W)) boxY -= speed * dt; // Y grows downward
        if (ech::IsKeyHeld(ech::KEY_S)) boxY += speed * dt;

        ech::StartDrawing();

        ech::DrawRectangle(boxX, boxY, 40, 40, ech::RED);

        ech::EndDrawing();
    }

    ech::CloseWindow();
    ech::Shutdown();
    return 0;
}
