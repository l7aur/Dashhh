#include "raylib.h"
#include <iostream>

#define BACKGROUND_COLOR BLACK
#define STARTING_VELOCITY 0.0f
#define UPWARD_IMPULSE -2'000.0f
#define GRAVITY 8'000.0f
#define HAZARD_COOLDOWN_TIME 3.0f

const int window_height = 600;
const int window_width = 800;
const float update_time = 1.0 / 12.0;

int main() {
    InitWindow(window_width, window_height, "DASHHH");
    SetTargetFPS(80);

    bool stop = false;
    float current_velocity = STARTING_VELOCITY;
    float running_time = 0.0;
    float hazard_cooldown_time = 0.0f;

    //hazard variables
    int hazard_frame = 0;
    int hazard_frame_go_down = 0;
    Texture2D hazard = LoadTexture("textures\\12_nebula_spritesheet.png");
    Rectangle hazard_rect;
    hazard_rect.width = hazard.width / 8;
    hazard_rect.height = hazard.height / 8;
    hazard_rect.x = hazard_rect.y = 0;
    Vector2 hazard_pos;
    hazard_pos.x = window_width;
    hazard_pos.y = window_height - hazard_rect.height;
    int hazard_ox_velocity = -10; 

    //scarfy variables
    int scarfy_frame = 0;
    Texture2D scarfy = LoadTexture("textures\\scarfy.png");
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
        running_time += dT;
        hazard_cooldown_time += dT;

        if(IsKeyPressed(KEY_SPACE) && scarfy_pos.y == window_height - scarfy.height) {
            current_velocity = UPWARD_IMPULSE;
        }
        current_velocity += GRAVITY * dT;

        scarfy_pos.y += current_velocity * dT;
        if(scarfy_pos.y > window_height - scarfy.height) {
            scarfy_pos.y = window_height - scarfy.height;
            current_velocity = STARTING_VELOCITY;
        }
        if(scarfy_pos.y < 0) {
            scarfy_pos.y = 0;
            current_velocity = STARTING_VELOCITY;
        }
        hazard_pos.x += hazard_ox_velocity;
        
        scarfy_rect.x = scarfy_frame * scarfy.width / 6;
        hazard_rect.x = hazard_frame * hazard.width / 8;
        hazard_rect.y = hazard_frame_go_down * hazard.height / 8;
        DrawTextureRec(scarfy, scarfy_rect, scarfy_pos, WHITE);
        DrawTextureRec(hazard, hazard_rect, hazard_pos, WHITE);
        if(running_time >= update_time) {
            scarfy_frame = (scarfy_frame + 1) % 6;
            hazard_frame = (hazard_frame + 1) % 8;
            if(hazard_frame == 0) 
               hazard_frame_go_down = (hazard_frame_go_down + 1) % 7;
            running_time = 0.0f;
        }

        if(hazard_cooldown_time >= HAZARD_COOLDOWN_TIME) {
            hazard_cooldown_time = 0.0f;
            hazard_pos.x = window_width;
        }

        stop = WindowShouldClose();
        EndDrawing();
    }

    UnloadTexture(scarfy);
    UnloadTexture(hazard);
    CloseWindow();

    return 0;
}