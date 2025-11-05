// main.cpp
#include "echlib.hpp"
#include <iostream>
#include <vector>

int main() {
    // Window + graphics
    const int SCREEN_W = 800;
    const int SCREEN_H = 600;
    ech::CreateWindow(SCREEN_W, SCREEN_H, "Echlib - Movement + Spikes Demo");
    ech::InitGraphics();
    ech::SetFpsLimit(60);

    // Player
    float px = 100.0f;
    float py = 100.0f;
    const float pw = 48.0f;
    const float ph = 48.0f;
    const float speed = 300.0f; // pixels per second

    // Ground
    const float gx = -400.0f;
    const float gy = (float)SCREEN_H - 60.0f;
    const float gw = 2400.0f;
    const float gh = 60.0f;

    // Camera smoothing / settings
    const float camLerp = 0.12f;

    // Physics
    float vy = 0.0f;
    const float gravity = 1500.0f;
    const float jumpVelocity = -450.0f;
    bool onGround = false;

    // Spike data
    struct Spike {
        float x, y, size;
    };

    std::vector<Spike> spikes = {
        {400, gy - 32, 32},
        {600, gy - 32, 32},
        {850, gy - 32, 32},
        {1200, gy - 32, 32},
        {1500, gy - 32, 32}
    };

    // Main loop
    while (!ech::WindowShouldClose()) {
        float dt = ech::GetDeltaTime();
        if (dt <= 0.0f) dt = 1.0f / 60.0f;

        // --- Input & movement ---
        float vx = 0.0f;
        if (ech::IsKeyHeld(ech::KEY_D) || ech::IsKeyHeld(ech::KEY_D)) vx += speed;
        if (ech::IsKeyHeld(ech::KEY_A) || ech::IsKeyHeld(ech::KEY_A)) vx -= speed;

        // Jump (space)
        if ((ech::IsKeyPressed(ech::KEY_SPACE) || ech::IsKeyPressed(ech::KEY_W)) && onGround) {
            vy = jumpVelocity;
            onGround = false;
        }

        // Apply velocities
        px += vx * dt;
        vy += gravity * dt;
        py += vy * dt;

        // --- Collision with ground ---
        if (ech::CheckCollision(px, py, pw, ph, gx, gy, gw, gh)) {
            py = gy - ph;
            vy = 0.0f;
            onGround = true;
        }
        else {
            onGround = false;
        }

        // --- Spike collisions ---
        bool hitSpike = false;
        for (auto& s : spikes) {
            // Approximate spike as a small square hitbox for simplicity
            if (ech::CheckCollision(px, py, pw, ph, s.x, s.y, s.size, s.size)) {
                hitSpike = true;
                break;
            }
        }

        if (hitSpike) {
            // Reset player to start
            px = 100.0f;
            py = 100.0f;
            vy = 0.0f;
            std::cout << "Ouch! Hit a spike!\n";
        }

        // --- Update camera ---
        ech::UpdateCamera(px + pw * 0.5f, py + ph * 0.5f, camLerp, (float)SCREEN_W, (float)SCREEN_H);

        // --- Rendering ---
        ech::StartDrawing();
        ech::ClearBackground(ech::BEIGE);

        // Ground
        ech::DrawRectangle(gx, gy, gw, gh, ech::LIGHT_BLUE);

        // Spikes (red triangles)
        for (auto& s : spikes) {
            ech::DrawTriangle(s.x, s.y + s.size, s.size, -s.size, ech::RED);
        }

        // Player
        ech::DrawRectangle(px, py, pw, ph, ech::LIGHT_GREEN);

        // Shadow under player
        ech::DrawRectangle(px + 6.0f, gy - 6.0f, pw, 6.0f, { 0, 0, 0, 0.15f });

        ech::EndDrawing();
    }

    ech::CloseWindow();
    return 0;
}
