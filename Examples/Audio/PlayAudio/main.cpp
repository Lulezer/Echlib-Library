#include "echlib.h"  // Include raylib header
#include <iostream>

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    // Load sound files
    Sound fxWav = LoadSound("song.wav");  // Load sound file (change extension if needed)

    ech::MakeWindow(screenWidth, screenHeight, "raylib [audio] example - sound loading and playing");

    InitAudioDevice();  // Initialize audio device

    // Set target FPS
    ech::SetTargetFps(60);

    PlaySound(fxWav);


    // Main game loop
    while (!ech::WindowShouldClose()) {  // Detect window close button or ESC key

        // Draw
        ech::StartDrawing();

        ech::ClearBackground(ech::WHITE);

        ech::EndDrawing();
    }

    // De-initialization
    UnloadSound(fxWav);  // Unload sound data
    CloseAudioDevice();   // Close audio device
    ech::CloseWindow();        // Close window and OpenGL context

    return 0;
}
