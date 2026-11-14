#include <echlib.h> // Window, drawing and input
#include <raudio.h> // Audio. echlib does not include this for you

int main()
{
    int windowWidth = 640;
    int windowHeight = 360;

    ech::CreateWindow(windowWidth, windowHeight, "Audio example with echlib");
    ech::SetFpsLimit(60);

    InitAudioDevice(); // Initialize the audio device before loading any sound

    Sound soundEffect = LoadSound("song.wav");              // A short sound, kept in memory
    Music backgroundMusic = LoadMusicStream("sound.mp3");   // A long track, streamed from disk
    PlayMusicStream(backgroundMusic);

    ech::ClearBackground(ech::WHITE);

    while (!ech::WindowShouldClose())
    {
        // A streamed track needs its buffers refilled every frame or it goes silent
        UpdateMusicStream(backgroundMusic);

        if (ech::IsKeyPressed(ech::KEY_SPACE)) // Play the sound effect
        {
            PlaySound(soundEffect);
        }

        if (ech::IsKeyPressed(ech::KEY_P)) // Pause or resume the music
        {
            if (IsMusicPlaying(backgroundMusic))
                PauseMusicStream(backgroundMusic);
            else
                ResumeMusicStream(backgroundMusic);
        }

        if (ech::IsKeyPressed(ech::KEY_S)) // Stop the music
        {
            StopMusicStream(backgroundMusic);
        }

        ech::StartDrawing();

        // Draw your game here

        ech::EndDrawing();
    }

    UnloadSound(soundEffect);
    UnloadMusicStream(backgroundMusic);
    CloseAudioDevice();
    ech::CloseWindow();
    ech::Shutdown();

    return 0;
}
