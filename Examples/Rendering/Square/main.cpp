#include <echlib.h> // include echlib

int main()
{
	int WindowWidth = 640; // Define a width for the window
	int windowHeight = 360; // Define a height for the window
	int RectangleX = 320;// Define the X Position for the Rectangle
	int RectangleY = 180; // Define the Y Postion for the Rectangle
	int RectangleWidth = 50;// Define a width for the Rectangle
	int RectangleHeight = 50; // Define a height for the Rectangle


	MakeWindow(WindowWidth, windowHeight, "Window example with echlib"); // Create the window
	SetTargetFps(60); // You dont have to put this for the window to work but you should so that the game always run at 60fps.

	while (!WindowShouldClose())
	{
		StartDrawing();  // Start Drawing the window

		DrawRectangle(RectangleX, RectangleY, RectangleWidth, RectangleHeight, RED);

		ClearBackground(WHITE); // Clear the Background With a color

		EndDrawing(); // End Drawing The window 
	}

	CloseWindow(); // Close Window
	return 0;

}