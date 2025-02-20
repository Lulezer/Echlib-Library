#include <echlib.h> // include echlib

int main()
{
	int WindowWidth = 640; // Define a width for the window
	int windowHeight = 360; // Define a height for the window

	ech::MakeWindow(WindowWidth, windowHeight, "Window example with echlib"); // Create the window
	ech::SetTargetFps(60); // You dont have to put this for the window to work but you should so that the game always run at 60fps.

	while (!ech::WindowShouldClose())
	{
		ech::StartDrawing();  // Start Drawing the window

		ech::ClearBackground(ech::WHITE); // Clear the Background With a color

		ech::EndDrawing(); // End Drawing The window 
	}

	ech::CloseWindow(); // Close Window
	return 0;

}
