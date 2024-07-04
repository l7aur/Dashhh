#include "raylib.h"
#include <iostream>

#define BACKGROUND_COLOR BLACK
#define STARTING_VELOCITY 0
#define UPWARD_IMPULSE -200
#define GRAVITY 100

const int window_height = 600;
const int window_width = 800;

int main() {
    InitWindow(window_width, window_height, "DASHHH");
    SetTargetFPS(80);

    bool stop = false;
    int current_velocity = STARTING_VELOCITY;

    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    Rectangle scarfy_rect;
    scarfy_rect.width = scarfy.width / 6;
    scarfy_rect.height = scarfy.height;
    scarfy_rect.x = scarfy_rect.y = 0;
    Vector2 scarfy_pos;
    scarfy_pos.x = (window_width - scarfy_rect.width) / 2;
    scarfy_pos.y = window_height - scarfy_rect.height;

    while(!stop) {
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);

        float dT = GetFrameTime();

        if(IsKeyPressed(KEY_SPACE) && scarfy_pos.y == window_height - scarfy.height) {
            current_velocity = UPWARD_IMPULSE;
        }

        scarfy_pos.y += current_velocity * dT;
        if(scarfy_pos.y > window_height - scarfy.height) {
            scarfy_pos.y = window_height - scarfy.height;
            current_velocity = STARTING_VELOCITY;
        }
        if(scarfy_pos.y < 0) {
            scarfy_pos.y = 0;
            current_velocity = STARTING_VELOCITY;
        }

        DrawTextureRec(scarfy, scarfy_rect, scarfy_pos, WHITE);
        current_velocity += GRAVITY * dT;

        stop = WindowShouldClose();
        EndDrawing();
    }

    UnloadTexture(scarfy);
    CloseWindow();

    return 0;
}