#include "chip8.h"
#include "raylib.h"
#include <stdio.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <path_to_rom>\n", argv[0]);
        return 1;
    }

    Chip8 c;
    init_chip8(&c);
    load_rom(&c, argv[1]);

    const int scale = 10; //ch8 screen too smol , need scale
    const int width = SW * scale;
    const int height = SH * scale;
    
    InitWindow(width, height, "CHIP-8 Emulator");
    SetTargetFPS(60); 

    while (!WindowShouldClose()) {
        //mapp key from raylib def  to CHIP-8 Keypad
        c.keypad[0x1] = IsKeyDown(KEY_ONE);
        c.keypad[0x2] = IsKeyDown(KEY_TWO);
        c.keypad[0x3] = IsKeyDown(KEY_THREE);
        c.keypad[0xC] = IsKeyDown(KEY_FOUR);

        c.keypad[0x4] = IsKeyDown(KEY_Q);
        c.keypad[0x5] = IsKeyDown(KEY_W);
        c.keypad[0x6] = IsKeyDown(KEY_E);
        c.keypad[0xD] = IsKeyDown(KEY_R);

        c.keypad[0x7] = IsKeyDown(KEY_A);
        c.keypad[0x8] = IsKeyDown(KEY_S);
        c.keypad[0x9] = IsKeyDown(KEY_D);
        c.keypad[0xE] = IsKeyDown(KEY_F);

        c.keypad[0xA] = IsKeyDown(KEY_Z);
        c.keypad[0x0] = IsKeyDown(KEY_X);
        c.keypad[0xB] = IsKeyDown(KEY_C);
        c.keypad[0xF] = IsKeyDown(KEY_V);

        //cpu run
        for (int i = 0; i < 10; i++) {
            emulate_cycle(&c);
        }

        //Decrement Timers
        if (c.DT > 0) c.DT--;
        if (c.ST > 0) c.ST--;

        //Drawing
        if (c.draw_flag) {
            BeginDrawing();
            ClearBackground(BLACK);
            
            for (int y = 0; y < SH; y++) {
                for (int x = 0; x < SW; x++) {
                    if (c.display[x + (y * SW)] != 0) {
                        DrawRectangle(x * scale, y * scale, scale, scale, WHITE); // scale,scakle cause pixels need 10x magnification 
                    }
                }
            }
            EndDrawing();
            c.draw_flag = 0; 
        } else{
            BeginDrawing();
            EndDrawing();
        }
    }
    CloseWindow();
    return 0;
}