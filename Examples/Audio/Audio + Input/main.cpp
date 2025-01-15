#include <echlib.h> // Include echlib for window and input management

int main()
{
    // Window setup
    int WindowWidth = 640; // Width of the window
    int WindowHeight = 360; // Height of the window
    MakeWindow(WindowWidth, WindowHeight, "Audio Example with echlib"); // Create a window
    SetTargetFps(60); // Set the target FPS for the game loop

    // Audio setup
    InitAudioDevice(); // Initialize the audio system
    Sound soundEffect = LoadSound("song.wav"); // Load a sound effect file (e.g., "example.wav")
    Music backgroundMusic = LoadMusicStream("sound.mp3"); // Load background music (e.g., "background.mp3")
    PlayMusicStream(backgroundMusic); // Start playing the background music


    // Main game loop
    while (!WindowShouldClose()) // Check if the window should close
    {
        StartDrawing(); // Begin drawing the current frame

        // Update the music stream to keep it playing
        UpdateMusicStream(backgroundMusic);

        // Detect key presses to control the audio
        if (IsKeyPressed(KEY_SPACE)) // Play the sound effect when SPACE is pressed
        {
            PlaySound(soundEffect);
        }

        if (IsKeyPressed(KEY_P)) // Pause or resume the background music with P
        {
            if (IsMusicPlaying(backgroundMusic))
                PauseMusicStream(backgroundMusic);
            else
                ResumeMusicStream(backgroundMusic);
        }

        if (IsKeyPressed(KEY_S)) // Stop the background music with S
        {
            StopMusicStream(backgroundMusic);
        }

        ClearBackground(WHITE); // Clear the window with a white background

        EndDrawing(); // End drawing the current frame
    }

    // Cleanup resources
    UnloadSound(soundEffect); // Unload the sound effect
    UnloadMusicStream(backgroundMusic); // Unload the background music
    CloseAudioDevice(); // Close the audio system
    CloseWindow(); // Close the window

    return 0; // Exit the program
}
