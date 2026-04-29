#include<stdint.h>

typedef struct {
    uint8_t ram[4096]; //8bit=1byte => 4*1024 bytes=4KB
    uint8_t V[16]; // V0-VF registers
    uint16_t I; // memory address storage register 
    uint16_t PC;// program counter register
    uint16_t SP; // stack pointer
    uint16_t stack[16]; // 16 level stack totalling 32 byte
    uint8_t keypad[16]; // hex keypad 0-F
    uint8_t display[32][64]; // 32 rows(height) 64 columns(width) 
    uint8_t ST,DT; //sound registers ST=Sound timer DT=Delay Timer 

}hardware;