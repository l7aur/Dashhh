#include "raylib.h"
#include <iostream>
#include <unistd.h>

//macros
#define BACKGROUND_COLOR BLACK
#define STARTING_VELOCITY 0.0f
#define UPWARD_IMPULSE -2'000.0f
#define GRAVITY 8'000.0f
#define HAZARD_COOLDOWN_TIME 3.0f
#define BACKGROUND1_VELOCITY 200.0f
#define BACKGROUND2_VELOCITY 400.0f
#define BACKGROUND3_VELOCITY 800.0f
#define BACKGROUND_SCALING_FACTOR 3.3f
#define MY_TINT WHITE
#define COUNTDOWN 3.0f
#define SCORE_SIZE 10

//types
struct animation_data {
    Rectangle rectangle;
    Vector2 position;
    int frame;
    float running_time;
};

//constants
const int window_height = 600;
const int window_width = 800;
const float scarfy_update_time = 1.0 / 12.0;
const float hazard_update_time = 1.0 / 32.0;
const float hazard_padding = 40.0f;

//checks if something is on the ground
bool is_on_ground(animation_data data) {
    return (data.position.y == window_height - data.rectangle.height);
}

//animates a texture
void animate(Texture2D texture, animation_data& data, float update_time) {
    data.rectangle.x = data.frame * data.rectangle.width;
    DrawTextureRec(texture, data.rectangle, data.position, MY_TINT);
    if(data.running_time >= update_time) {
        data.frame = (data.frame + 1) % ((int)(texture.width / data.rectangle.width));
        data.running_time = 0.0f;
    }
}

//computes the new y position for a hazard
void compute_hazard_line(animation_data& hazard_data, int posY) {
    hazard_data.position.x = window_width;
    if(rand() % 2)
        hazard_data.position.y = posY;
    else
        hazard_data.position.y = window_height - hazard_data.rectangle.height;
        
}

void move_background(Texture2D background, float& x, float delta_time, float velocity, float scaling_factor) {
    DrawTextureEx(background, {x, 0.0f}, 0.0f, BACKGROUND_SCALING_FACTOR, MY_TINT);
    DrawTextureEx(background, {x + background.width * BACKGROUND_SCALING_FACTOR, 0.0f}, 0.0f, BACKGROUND_SCALING_FACTOR, MY_TINT);
    x -= velocity * delta_time;
    if(x + background.width * BACKGROUND_SCALING_FACTOR <= 0)
        x = 0.0f;
}

void get_score(char * score, int x) {
    if(x == 0) return;
    int dim = SCORE_SIZE;
    while(x) {
        score[--dim] = x % 10 + '0';
        x /= 10;
    }
}

int main() {
    InitWindow(window_width, window_height, "DASHHH");
    SetTargetFPS(80);

    //miscellaneous variables
    float current_velocity = STARTING_VELOCITY;
    float hazard_cooldown_time = 0.0f; //how much until next hazard appears
    float countdown = COUNTDOWN;
    bool game_ended = false;
    int text_posY = -120;
    unsigned int expect_to_end = 0;
    char score[SCORE_SIZE + 1];
    score[SCORE_SIZE] = '\0';
    for(int i = 0; i < SCORE_SIZE; i++)
        score[i] = '0';
    unsigned int score_points = 0;
    float difficulty = 1.0f;

    //hazard variables
    Texture2D hazard = LoadTexture("textures\\12_nebula_spritesheet.png");
    float hazard_ox_velocity = -1000; //must be negative 
    animation_data hazard_data = {
        {0.0f, 0.0f, hazard.width / 8.0f, hazard.height / 8.0f},    //rectangle
        {window_width, window_height - hazard_data.rectangle.height},   //position
        0,  //frame
        0.0f    //running time
    };

    //scarfy variables
    Texture2D scarfy = LoadTexture("textures\\scarfy.png");
    animation_data scarfy_data = {
        {0.0f, 0.0f, scarfy.width / 6.0f, (float)scarfy.height},    //rectangle
        {(window_width - scarfy_data.rectangle.width) / 2 - 100, window_height - scarfy_data.rectangle.height},   //position
        0, //frame
        0.0f //running time
    };
    
    //background
    Texture2D background1 = LoadTexture("textures\\foreground.png");
    float bg1_x = 0.0f;

    Texture2D background2 = LoadTexture("textures\\back-buildings.png");
    float bg2_x = 0.0f;

    Texture2D background3 = LoadTexture("textures\\far-buildings.png");
    float bg3_x = 0.0f;

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);

        float dT = GetFrameTime() * difficulty;

        //draw and animate background
        move_background(background3, bg1_x, dT, BACKGROUND1_VELOCITY, BACKGROUND_SCALING_FACTOR);
        move_background(background2, bg2_x, dT, BACKGROUND2_VELOCITY, BACKGROUND_SCALING_FACTOR);
        move_background(background1, bg3_x, dT, BACKGROUND3_VELOCITY, BACKGROUND_SCALING_FACTOR);

        //no double jumps
        if(is_on_ground(scarfy_data)) {
            scarfy_data.running_time += dT;
            if(IsKeyPressed(KEY_SPACE))
                current_velocity += UPWARD_IMPULSE;
        }

        //update timers
        hazard_data.running_time += dT;
        hazard_cooldown_time += dT;
        if(countdown > 0) countdown -= dT;

        //update velocities
        if(!game_ended) {
            current_velocity += GRAVITY * dT;
            if(countdown <= 0 && hazard_data.position.x + hazard_data.rectangle.width > 0) 
                hazard_data.position.x += hazard_ox_velocity * dT;
            scarfy_data.position.y += current_velocity * dT;
        }

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
        if(countdown <= 0) animate(hazard, hazard_data, hazard_update_time);

        //compute new hazard if needed
        if(!game_ended && hazard_cooldown_time >= HAZARD_COOLDOWN_TIME) {
            compute_hazard_line(hazard_data, window_height - scarfy_data.rectangle.height - hazard_data.rectangle.height);
            hazard_cooldown_time = 0.0f;

            //compute score and increase difficulty
            score_points++;
            difficulty += 0.05f;
        }

        //collision detection
        if(!game_ended && CheckCollisionRecs(Rectangle{scarfy_data.position.x, scarfy_data.position.y,
                                        scarfy_data.rectangle.width, scarfy_data.rectangle.height},
                              Rectangle{hazard_data.position.x + hazard_padding, hazard_data.position.y + hazard_padding,
                                        hazard_data.rectangle.width - hazard_padding, hazard_data.rectangle.height - hazard_padding} 
                            ) == true){
            game_ended = true;

            //compute score
            score_points--;
        }

        if(game_ended) {
            DrawText(" Game   over! \n\n\n\nYour score is\n\n\n\n", (window_width >> 2) + 20, text_posY, 64, WHITE);
            DrawText(score, (window_width >> 2) + 50, text_posY + 120, 64, WHITE);
            if(text_posY < window_height) text_posY += 2;
            if(text_posY > (window_height >> 1) && !expect_to_end)
                expect_to_end = 1;
        }

        //force stop
        if(expect_to_end > 0)
            expect_to_end++;
        if(expect_to_end > 700)
            break;

        //display score
        get_score(score, score_points);
        if(!game_ended) DrawText(score, 10, 10, 24, WHITE);
        EndDrawing();
    }

    UnloadTexture(scarfy);
    UnloadTexture(hazard);
    UnloadTexture(background1);
    UnloadTexture(background2);
    UnloadTexture(background3);
    CloseWindow();

    return 0;
}