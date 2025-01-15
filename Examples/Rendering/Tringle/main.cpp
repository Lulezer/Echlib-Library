#include <echlib.h> // include echlib

int main()
{
	int WindowWidth = 640; // Define a width for the window
	int windowHeight = 360; // Define a height for the window
	int TringleX = 320;// Define the X Position for the Tringle
	int TringleY = 180; // Define the Y Postion for the Tringle
	int TringleWidth = 50;// Define a width for the Tringle
	int TringleHeight = 50; // Define a height for the Tringle


	MakeWindow(WindowWidth, windowHeight, "Window example with echlib"); // Create the window
	SetTargetFps(60); // You dont have to put this for the window to work but you should so that the game always run at 60fps.

	while (!WindowShouldClose())
	{
		StartDrawing();  // Start Drawing the window

		DrawTriangle(TringleX, TringleY, TringleWidth, TringleHeight, RED);

		ClearBackground(WHITE); // Clear the Background With a color

		EndDrawing(); // End Drawing The window 
	}

	CloseWindow(); // Close Window
	return 0;

}