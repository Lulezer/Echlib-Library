#include <echlib.h> // Include the echlib library to use its functions for creating and managing windows, input, and drawing.

int main()
{
    // Set the dimensions of the window
    int WindowWidth = 640; // Width of the window in pixels
    int WindowHeight = 360; // Height of the window in pixels

    // Variable to store the current background color state
    // 1 = White, 2 = Blue, 3 = Red, 4 = Green
    int color = 1;

    // Create a window with the specified dimensions and a title
    MakeWindow(WindowWidth, WindowHeight, "Window example with echlib");

    // Set the game loop to run at 60 frames per second
    SetTargetFps(60);

    // Main game loop: runs until the user closes the window
    while (!WindowShouldClose()) // Check if the window should close
    {
        StartDrawing(); // Begin the drawing phase for the current frame

        // Check if the right arrow key is pressed (not held)
        if (IsKeyHeld(KEY_RIGHT))
        {
            color += 1; // Increment the color variable to cycle to the next color
        }

        // Update the background color based on the current color state
        if (color == 1)
        {
            ClearBackground(WHITE); // Set background to white
        }
        else if (color == 2)
        {
            ClearBackground(BLUE); // Set background to blue
        }
        else if (color == 3)
        {
            ClearBackground(RED); // Set background to red
        }
        else if (color == 4)
        {
            ClearBackground(GREEN); // Set background to green
        }
        else if (color >= 5) // Reset the color cycle back to 1 after green
        {
            color = 1; // Start again from white
        }

        EndDrawing(); // End the drawing phase for the current frame
    }

    // Close the window and release resources when the loop ends
    CloseWindow();

    return 0; // Exit the program successfully
}
