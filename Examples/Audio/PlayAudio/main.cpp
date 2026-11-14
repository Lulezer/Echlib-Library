#include <echlib.h> // Window, drawing and input
#include <raudio.h> // Audio. echlib does not include this for you

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    ech::CreateWindow(screenWidth, screenHeight, "echlib [audio] example - sound loading and playing");
    ech::SetFpsLimit(60);

    InitAudioDevice(); // Initialize the audio device before loading any sound

    Sound fxWav = LoadSound("song.wav"); // Load a sound file (.wav, .ogg, .flac, .mp3)
    PlaySound(fxWav);

    ech::ClearBackground(ech::WHITE);

    while (!ech::WindowShouldClose())
    {
        ech::StartDrawing();

        // Draw your game here

        ech::EndDrawing();
    }

    UnloadSound(fxWav);  // Unload the sound data
    CloseAudioDevice();  // Close the audio device
    ech::CloseWindow();  // Destroy the window
    ech::Shutdown();     // Shut GLFW down

    return 0;
}
