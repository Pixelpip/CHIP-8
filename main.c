#include "raylib.h"

int main(){
    const int width=640;
    const int height=320;
    InitWindow(width, height, "chip8 window");

    SetTargetFPS(60); 


    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("first window!", 190, 200, 20, BLACK);
        EndDrawing();

    }
    CloseWindow();
    
}