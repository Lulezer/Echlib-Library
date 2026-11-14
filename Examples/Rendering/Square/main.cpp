#include <echlib.h> // include echlib

int main()
{
	int windowWidth = 640;  // Define a width for the window
	int windowHeight = 360; // Define a height for the window

	float rectangleX = 320.0f;     // X position of the rectangle (its top-left corner)
	float rectangleY = 180.0f;     // Y position of the rectangle
	float rectangleWidth = 50.0f;  // Width of the rectangle
	float rectangleHeight = 50.0f; // Height of the rectangle

	ech::CreateWindow(windowWidth, windowHeight, "Square example with echlib");
	ech::SetFpsLimit(60);

	ech::ClearBackground(ech::WHITE); // The colour StartDrawing() clears with

	while (!ech::WindowShouldClose())
	{
		ech::StartDrawing();

		// Draw after StartDrawing(), otherwise the clear wipes it away
		ech::DrawRectangle(rectangleX, rectangleY, rectangleWidth, rectangleHeight, ech::RED);

		ech::EndDrawing();
	}

	ech::CloseWindow();
	ech::Shutdown();
	return 0;
}
