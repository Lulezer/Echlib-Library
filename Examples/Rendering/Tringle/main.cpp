#include <echlib.h> // include echlib

int main()
{
	int windowWidth = 640;  // Define a width for the window
	int windowHeight = 360; // Define a height for the window

	float triangleX = 320.0f;     // X position of the triangle's bottom-left corner
	float triangleY = 180.0f;     // Y position of the triangle's baseline
	float triangleWidth = 50.0f;  // Width of the baseline
	float triangleHeight = 50.0f; // Positive height puts the apex below the baseline,
	                              // negative height flips the triangle upward

	ech::CreateWindow(windowWidth, windowHeight, "Triangle example with echlib");
	ech::SetFpsLimit(60);

	ech::ClearBackground(ech::WHITE);

	while (!ech::WindowShouldClose())
	{
		ech::StartDrawing();

		ech::DrawTriangle(triangleX, triangleY, triangleWidth, triangleHeight, ech::RED);

		ech::EndDrawing();
	}

	ech::CloseWindow();
	ech::Shutdown();
	return 0;
}
