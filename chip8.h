#pragma once 


#include<stdint.h>



#define MEM_SIZE 4096
#define NUM_REGISTER 16
#define STACK_SIZE 16
#define NUM_KEYS 16
#define SW 64
#define SH 32
#define FONT_SIZE 80
#define FONT_ADDR 0x50 
#define PGM_START 0x200


typedef struct {
    uint8_t ram[MEM_SIZE]; //8bit=1byte , 4*1024 bytes=4KB
    uint8_t V[NUM_REGISTER]; // V0-VF registers
    uint16_t I; // memory address storage register 
    uint16_t PC;// program counter register
    uint16_t SP; // stack pointer
    uint16_t stack[STACK_SIZE]; // 16 level stack totalling 32 byte
    uint8_t keypad[NUM_KEYS]; // hex keypad 0-F
    uint8_t display[SW*SH]; // 32 rows(height) 64 columns(width) 
    uint8_t ST,DT; //sound registers ST=Sound timer DT=Delay Timer 
    uint16_t opcode; //current opcode 
    uint8_t draw_flag; //should draw or not


}Chip8;

void init_chip8(Chip8 *chip8); //initialize the system
void load_rom(Chip8 *c,const char *filename);  //loading the rom 
void emulate_cycle(Chip8 *c); //cpu cycle emulation

