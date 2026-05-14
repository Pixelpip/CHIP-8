#include "chip8.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

static const uint8_t font[FONT_SIZE]={
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void init_chip8(Chip8 *chip8){
    memset(chip8,0,sizeof(chip8)); //set the whole ram to 0 
    chip8->PC=PGM_START; //PC set to the address 0x200 where programs start
    memcpy(&chip8->ram[FONT_ADDR],font,FONT_SIZE); //fonts loaded into the ram 
}

void load_rom(Chip8 *chip8, const char *filename){
    FILE *fp=fopen(filename,"rb");
    if(!fp){
        fprintf(stderr,"error:could not open file:%s\n",filename);
        return;
    }
    size_t bytes_read=fread(&chip8->ram[PGM_START],1,MEM_SIZE-PGM_START,fp);

    if(bytes_read==0){
        fprintf(stderr,"error: ROM %s is empty or DNE",filename);
    }
    else{
        fprintf(stderr,"Read sucess on ROM %s with size %zu in bytes",filename,bytes_read);
    }

}
void emulate_cycle(Chip8 *chip8){
    //opcode merging from 8bit to 16bit 
    uint16_t opcode=(chip8->ram[chip8->PC]<<8)|(chip8->ram[chip8->PC+1]); //first half of opcode leftshift by 8bits and other half accomodates the other 8bits 
    chip8->PC+=2;
    

    //splitting the opcode from 16bit into chunks 
    uint8_t X=(opcode&0x0F00)>>8; //fetches the bits from 9-12 , right shifted to fit into a 8 bit
    uint8_t Y=(opcode&0x00F0)>>4; //same but 5-8
    uint8_t n=(opcode&0x000F); //last one , n is the 4bit value
    uint8_t kk=(opcode&0x0FF); //whole byte fetch , kk is 8bit
    uint8_t nnn=(opcode&0x0FFF);//nnn is 12bit

    switch(opcode&0x00F){  //switch to determine the type of the opcode using opcodes first 4 bits
        case 0x0000:
            switch(opcode&0x00FF){
                case 0x00E0:
                    memset(chip8->display,0,SW*SH);
                    chip8->draw_flag=1;
                    break;
                case 0x00EE:
                    chip8->PC = chip8->stack[chip8->SP--];
                    break;
                default:
                    fprintf(stderr,"Wrong opcode");
                    break;
            }
        break;
        case 0x1000:
            chip8->PC=nnn;
            break;
        case 0x2000:
            chip8->stack[++chip8->SP]=chip8->PC;
            break;
        case 0x3000:
            if(chip8->V[X]==kk) chip8->PC+=2;
            break;
        case 0x4000:
            if(chip8->V[X]!=kk) chip8->PC+=2;
            break;
        case 0x5000:
            if(chip8->V[X]==chip8->V[Y]) chip8->PC+=2;
            break;
        case 0x6000:
            if(chip8->V[X]==kk) chip8->V[X]=kk;
            break;
        case 0x7000:
            chip8->V[X]+=kk;
            break;
        case 0x8000:
            switch(n){
                case 0x0:
                    chip8->V[X]=chip8->V[Y];
                    break;
                case 0x1:
                    chip8->V[X]|=chip8->V[Y];
                    break;
                case 0x2:
                    chip8->V[X]&=chip8->V[Y];
                    break;
                case 0x3:
                    chip8->V[X]^=chip8->V[Y];
                    break;
                case 0x4:
                    uint16_t sum=chip8->V[X]+chip8->V[Y];
                    chip8->V[0xF]=(sum>0xFF)?1:0;
                    chip8->V[X]=(sum&0xFF);
                    break;
                case 0x5:
                    uint8_t flag=(chip8->V[X] >= chip8->V[Y]) ? 1 : 0;
                    chip8->V[X]=chip8->V[X] - chip8->V[Y];
                    chip8->V[0xF]=flag;
                    break;
                case 0x6:
                    chip8->V[0xF]=(chip8->V[X])&0x1;
                    chip8->V[X]>>=1; //right shifting by one place means dividing by 2
                    break;

            }

    }



}

