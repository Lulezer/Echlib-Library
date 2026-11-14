#include <echlib.h> // include echlib

int main()
{
	int windowWidth = 640;  // Define a width for the window
	int windowHeight = 360; // Define a height for the window

	ech::CreateWindow(windowWidth, windowHeight, "Window example with echlib"); // Create the window
	ech::SetFpsLimit(60); // Optional, but it keeps the game from running as fast as it can

	// ClearBackground() sets the colour that the next StartDrawing() clears with,
	// so setting it once before the loop is enough for a background that never changes.
	ech::ClearBackground(ech::WHITE);

	while (!ech::WindowShouldClose())
	{
		ech::StartDrawing(); // Clear the window and upload the camera matrices

		// Draw your game here

		ech::EndDrawing(); // Swap buffers, poll events and wait for the FPS limit
	}

	ech::CloseWindow(); // Destroy the window
	ech::Shutdown();    // Shut GLFW down
	return 0;
}
