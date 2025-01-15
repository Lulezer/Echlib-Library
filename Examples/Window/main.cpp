#include <echlib.h> // include echlib

int main()
{
	int WindowWidth = 640; // Define a width for the window
	int windowHeight = 360; // Define a height for the window

	MakeWindow(WindowWidth, windowHeight, "Window example with echlib"); // Create the window
	SetTargetFps(60); // You dont have to put this for the window to work but you should so that the game always run at 60fps.

	while (!WindowShouldClose())
	{
		StartDrawing();  // Start Drawing the window

		if (IsKeyPressed(KEY_RIGHT) // if the right arrow is Pressed not held then set color to blue
		{
			ClearBackground(BLUE); // Clear the Background With a color

		}

		ClearBackground(WHITE); // Clear the Background With a color

		EndDrawing(); // End Drawing The window 
	}

	CloseWindow(); // Close Window
	return 0;

}