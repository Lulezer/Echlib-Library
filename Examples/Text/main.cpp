// Text example: load a .ttf font once, then draw strings with it.
#include <echlib.h>
#include <string>

int main()
{
    ech::CreateWindow(640, 360, "Text example with echlib");
    ech::SetFpsLimit(60);

    // A Font bakes an atlas at one pixel height, so load one Font per size you need.
    // Point this at any .ttf next to your executable.
    ech::Font font("font.ttf", 32.0f);

    ech::Font smallFont;
    if (!smallFont.Load("font.ttf", 16.0f)) {
        // Load() returns false if the file is missing; drawing with it is a no-op
    }

    ech::ClearBackground(ech::BLACK);

    int frames = 0;

    while (!ech::WindowShouldClose())
    {
        frames++;

        ech::StartDrawing();

        // x, y is the text baseline, so y = 0 would put the text off the top edge
        ech::DrawText(font, "Hello from echlib!", 40.0f, 80.0f, ech::WHITE);
        ech::DrawText(font, "ASCII 32-126 only", 40.0f, 130.0f, ech::YELLOW);

        // Equivalent to the free function: font.Draw(...)
        smallFont.Draw("frame " + std::to_string(frames), 40.0f, 180.0f, ech::LIGHT_GREEN);

        ech::EndDrawing();
    }

    ech::CloseWindow();
    ech::Shutdown(); // Safe to call before the Font goes out of scope
    return 0;
}
