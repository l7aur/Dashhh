#include "raylib.h"
#include <iostream>

#define BACKGROUND_COLOR BLACK
#define STARTING_VELOCITY 0.0f
#define UPWARD_IMPULSE -2'000.0f
#define GRAVITY 8'000.0f
#define HAZARD_COOLDOWN_TIME 3.0f

struct animation_data {
    Rectangle rectangle;
    Vector2 position;
    int frame;
    float update_time;
    float running_time;
};

const int window_height = 600;
const int window_width = 800;
const float scarfy_update_time = 1.0 / 12.0;
const float hazard_update_time = 1.0 / 32.0;



int main() {
    InitWindow(window_width, window_height, "DASHHH");
    SetTargetFPS(80);

    bool stop = false;
    float current_velocity = STARTING_VELOCITY;
    float global_running_time = 0.0f;
    float hazard_cooldown_time = 0.0f; //how much until next hazard appears

    //hazard variables
    Texture2D hazard = LoadTexture("textures\\12_nebula_spritesheet.png");
    float hazard_ox_velocity = -1000; //must be negative 
    animation_data hazard_data = {
        {0.0f, 0.0f, hazard.width / 8.0f, hazard.height / 8.0f},    //rectangle
        {window_width, window_height - hazard_data.rectangle.height},   //position
        0,  //frame
        0.0f,   //update time
        0.0f    //running time
    };

    //scarfy variables
    Texture2D scarfy = LoadTexture("textures\\scarfy.png");
    animation_data scarfy_data = {
        {0.0f, 0.0f, scarfy.width / 6.0f, (float)scarfy.height},    //rectangle
        {(window_width - scarfy_data.rectangle.width) / 2, window_height - scarfy_data.rectangle.height},   //position
        0, //frame
        0.0f, //update time
        0.0f //running time
    };
    
    while(!stop) {
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);

        float dT = GetFrameTime();

        if(scarfy_data.position.y == window_height - scarfy.height)
            scarfy_data.running_time += dT;

        global_running_time += dT;
        hazard_cooldown_time += dT;

        if(IsKeyPressed(KEY_SPACE) && scarfy_data.position.y == window_height - scarfy.height) {
            current_velocity = UPWARD_IMPULSE;
        }
        current_velocity += GRAVITY * dT;

        scarfy_data.position.y += current_velocity * dT;
        if(scarfy_data.position.y > window_height - scarfy.height) {
            scarfy_data.position.y = window_height - scarfy.height;
            current_velocity = STARTING_VELOCITY;
        }
        if(scarfy_data.position.y < 0) {
            scarfy_data.position.y = 0;
            current_velocity = STARTING_VELOCITY;
        }
        hazard_data.position.x += hazard_ox_velocity * dT;
        
        scarfy_data.rectangle.x = scarfy_data.frame * scarfy.width / 6;
        hazard_data.rectangle.x = hazard_data.frame * hazard.width / 8;
        DrawTextureRec(scarfy, scarfy_data.rectangle, scarfy_data.position, WHITE);
        DrawTextureRec(hazard, hazard_data.rectangle, hazard_data.position, WHITE);
        if(scarfy_data.running_time >= scarfy_update_time) {
            scarfy_data.frame = (scarfy_data.frame + 1) % 6;
            scarfy_data.running_time = 0.0f;
        }

        if(global_running_time >= hazard_update_time) {
            global_running_time = 0.0f;
            hazard_data.frame = (hazard_data.frame + 1) % 8;
        }

        if(hazard_cooldown_time >= HAZARD_COOLDOWN_TIME) {
            hazard_cooldown_time = 0.0f;
            hazard_data.position.x = window_width;
        }

        stop = WindowShouldClose();
        EndDrawing();
    }

    UnloadTexture(scarfy);
    UnloadTexture(hazard);
    CloseWindow();

    return 0;
}