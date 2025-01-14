#include "echlib.h"  // Include raylib header
#include <iostream>

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    // Load sound files
    Sound fxWav = LoadSound(RESOURCES_PATH "song.wav");  // Load sound file (change extension if needed)

    MakeWindow(screenWidth, screenHeight, "raylib [audio] example - sound loading and playing");

    InitAudioDevice();  // Initialize audio device

    // Set target FPS
    SetTargetFps(60);

    PlaySound(fxWav);


    // Main game loop
    while (!WindowShouldClose()) {  // Detect window close button or ESC key

        // Draw
        StartDrawing();

        ClearBackground(WHITE);

        EndDrawing();
    }

    // De-initialization
    UnloadSound(fxWav);  // Unload sound data
    CloseAudioDevice();   // Close audio device
    CloseWindow();        // Close window and OpenGL context

    return 0;
}
