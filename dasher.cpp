#include "raylib.h"
#include <iostream>

//macros
#define BACKGROUND_COLOR BLACK
#define STARTING_VELOCITY 0.0f
#define UPWARD_IMPULSE -2'000.0f
#define GRAVITY 8'000.0f
#define HAZARD_COOLDOWN_TIME 3.0f

//types
struct animation_data {
    Rectangle rectangle;
    Vector2 position;
    int frame;
    float update_time;
    float running_time;
};

//constants
const int window_height = 600;
const int window_width = 800;
const float scarfy_update_time = 1.0 / 12.0;
const float hazard_update_time = 1.0 / 32.0;

//checks if something is on the ground
bool is_on_ground(animation_data data) {
    return (data.position.y == window_height - data.rectangle.height);
}

//animates a texture
void animate(Texture2D texture, animation_data& data, float update_time) {
    data.rectangle.x = data.frame * data.rectangle.width;
    DrawTextureRec(texture, data.rectangle, data.position, WHITE);
    if(data.running_time >= update_time) {
        data.frame = (data.frame + 1) % ((int)(texture.width / data.rectangle.width));
        data.running_time = 0.0f;
    }
}

//computes the new y position for a hazard
void compute_hazard_line(float& current_hazard_cooldown_time, animation_data& hazard_data, int posY) {
    if(current_hazard_cooldown_time >= HAZARD_COOLDOWN_TIME) {
            current_hazard_cooldown_time = 0.0f;
            hazard_data.position.x = window_width;
            if(rand() % 2)
                hazard_data.position.y = posY;
            else
                hazard_data.position.y = window_height - hazard_data.rectangle.height;
        }
}

int main() {
    InitWindow(window_width, window_height, "DASHHH");
    SetTargetFPS(80);

    //miscellaneous variables
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

        //no double jumps
        if(is_on_ground(scarfy_data)) {
            scarfy_data.running_time += dT;
            if(IsKeyPressed(KEY_SPACE))
                current_velocity += UPWARD_IMPULSE;
        }

        //update timers
        global_running_time += dT;
        hazard_cooldown_time += dT;

        //update velocities
        current_velocity += GRAVITY * dT;
        hazard_data.position.x += hazard_ox_velocity * dT;
        scarfy_data.position.y += current_velocity * dT;
        
        //cannot go below the viewport
        if(scarfy_data.position.y > window_height - scarfy.height) {
            scarfy_data.position.y = window_height - scarfy.height;
            current_velocity = STARTING_VELOCITY;
        }
        //cannot jump above the visible viewport
        if(scarfy_data.position.y < 0) {
            scarfy_data.position.y = 0;
            current_velocity = STARTING_VELOCITY;
        }

        //animate
        animate(scarfy, scarfy_data, scarfy_update_time);
        animate(hazard, hazard_data, hazard_update_time);

        //compute new hazard if needed
        compute_hazard_line(hazard_cooldown_time, hazard_data, window_height - scarfy_data.rectangle.height - hazard_data.rectangle.height);

        stop = WindowShouldClose();
        EndDrawing();
    }

    UnloadTexture(scarfy);
    UnloadTexture(hazard);
    CloseWindow();

    return 0;
}