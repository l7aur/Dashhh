#include "raylib.h"

#define BACKGROUND_COLOR BLACK

const int window_height = 600;
const int window_width = 800;

int main() {
    InitWindow(window_width, window_height, "DASHHH");
    SetTargetFPS(80);

    bool stop = false;
    while(!stop) {
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);
        stop = WindowShouldClose();

        EndDrawing();
    }

    return 0;
}