#include <echlib.h> // Include echlib for the window, input and drawing functions

int main()
{
    int windowWidth = 640;  // Width of the window in pixels
    int windowHeight = 360; // Height of the window in pixels

    // Which background colour we are on: 1 = white, 2 = blue, 3 = red, 4 = green
    int color = 1;

    ech::CreateWindow(windowWidth, windowHeight, "Pressing a key with echlib");
    ech::SetFpsLimit(60);

    while (!ech::WindowShouldClose())
    {
        // IsKeyPressed() is true for a single frame, on the frame the key goes down.
        // It only updates when you call it, so call it once per frame per key.
        if (ech::IsKeyPressed(ech::KEY_SPACE))
        {
            color += 1;
            if (color > 4) color = 1; // Back to white after green
        }

        ech::StartDrawing();

        // The colour set here is used by the next StartDrawing(), so a colour change
        // shows up one frame later.
        if (color == 1)      ech::ClearBackground(ech::WHITE);
        else if (color == 2) ech::ClearBackground(ech::LIGHT_BLUE);
        else if (color == 3) ech::ClearBackground(ech::RED);
        else                 ech::ClearBackground(ech::LIGHT_GREEN);

        ech::EndDrawing();
    }

    ech::CloseWindow();
    ech::Shutdown();
    return 0;
}
