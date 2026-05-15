#include "chip8.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

static const uint8_t font[FONT_SIZE]={
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

void init_chip8(Chip8 *chip8){
    memset(chip8,0,sizeof(Chip8)); //set the whole ram to 0 
    chip8->PC=PGM_START; //PC set to the address 0x200 where programs start
    for (int i = 0; i < FONT_SIZE; i++) { //setting fontsinto ram
        chip8->ram[FONT_ADDR + i] = font[i];
    }
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
    uint8_t kk=(opcode&0x00FF); //whole byte fetch , kk is 8bit
    uint16_t nnn=(opcode&0x0FFF);//nnn is 12bit

    switch(opcode&0xF000){  //switch to determine the type of the opcode using opcodes first 4 bits
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
            chip8->PC=nnn;
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
            chip8->V[X]=kk;
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
                    uint8_t f1=(chip8->V[X] >= chip8->V[Y]) ? 1 : 0;
                    chip8->V[X]=chip8->V[X] - chip8->V[Y];
                    chip8->V[0xF]=f1;
                    break;
                        
                case 0x6:
                    chip8->V[0xF]=(chip8->V[X])&0x1;
                    chip8->V[X]>>=1; //right shifting by one place means dividing by 2
                    break;
                case 0x7:
                    uint8_t f2=(chip8->V[Y] >= chip8->V[X]) ? 1 : 0;
                    chip8->V[X]=chip8->V[Y] - chip8->V[X];
                    chip8->V[0xF]=f2;
                    break;
                case 0xE:
                    chip8->V[0xF]=(chip8->V[X]&0x80)>>7; //check if MSB is 1 and take the MSB bit from 8th place to 0th place 
                    chip8->V[X]<<=1;
                    break;
                
            }
            break;
            case 0x9000:
                if(chip8->V[X]!=chip8->V[Y]) chip8->PC+=2;
                break;
            case 0xA000:
                chip8->I=nnn;
                break;
            case 0xB000:
                chip8->PC=nnn+chip8->V[0];
                break;
            case 0xC000:
                chip8->V[X]=(rand()%256)&kk;
                break;
            case 0xD000:
                uint8_t x_pos=chip8->V[X]%SW; //calc postions 
                uint8_t y_pos=chip8->V[Y]%SH;
                chip8->V[0xF]=0; //assume no collison 
                for(uint8_t row=0;row<n;row++){
                    uint8_t sprite_byte=chip8->ram[chip8->I+row]; //fetch the row of the sprite 
                    for(uint8_t col=0;col<8;col++){
                        if((sprite_byte&(0x80>>col))!=0){ //fetching each bit from the 8bits 
                            uint16_t screen_idx = (x_pos+col)+((y_pos+row)*SW); //calc position on screen 
                            if(screen_idx<SW*SH){ // if within bounds of screen 
                                if(chip8->display[screen_idx]==1)chip8->V[0xF]=1; //if pixel is already lit then collision occured
                                chip8->display[screen_idx]^=1; //whatever the if , change pixel state by xor 
                            }
                        }

                    }
                }
                chip8->draw_flag=1; //if this flag is 1 then starting drawing on screen using raylib 
                break;
            case 0xE000:
                switch(kk){
                    case 0x9E:
                        if(chip8->keypad[chip8->V[X]])chip8->PC+=2;
                        break;
                    case 0xA1:
                        if(!chip8->keypad[chip8->V[X]])chip8->PC+=2;
                        break;
                    default:
                        fprintf(stderr,"unknown 0XE000 opcode: 0x%04X\n",opcode);
                        break;
                }
                break;
            case 0xF000:
                switch(kk){
                    case 0x07:
                        chip8->V[X]=chip8->DT;
                        break;
                    case 0x0A:
                        uint8_t key_pres=0;
                        for(uint8_t i=0;i<16;i++){
                            if(chip8->keypad[i]!=0){
                                chip8->V[X]=i;
                                key_pres+=1;
                                break;
                            }
                        }
                        if(key_pres==0)chip8->PC-=2; //if key not presed then reduce PC and force fetch same opcode again from the ROM
                        break;
                    case 0x15:
                        chip8->DT=chip8->V[X];
                        break;
                    case 0x18:
                        chip8->ST=chip8->V[X];
                        break;
                    case 0x1E:
                        chip8->I+=chip8->V[X];
                        break;
                    case 0x29:
                        chip8->I=FONT_ADDR+((chip8->V[X]&0xF)*5); //15 different fonts only and each of width 5 
                        break;
                    case 0x33:
                        chip8->ram[chip8->I]   = chip8->V[X] / 100;
                        chip8->ram[chip8->I+1] = (chip8->V[X] / 10) % 10;
                        chip8->ram[chip8->I+2] = chip8->V[X] % 10;
                        break;
                    case 0x55:
                        for(uint8_t i=0;i<=X;i++){
                            chip8->ram[chip8->I+i]=chip8->V[i]; //copy data from V regs and store into ram from loc I 
                        }
                        break;
                    case 0x65:
                        for(uint8_t i=0;i<=X;i++){
                            chip8->V[i]=chip8->ram[chip8->I+i];
                        }
                        break;
                    default:
                        fprintf(stderr,"invalid opcode of range 0xF000(opcode:0x%04X\n)",opcode);
                        break;
                }
                break;
                default:
                    fprintf(stderr,"idk this opcode (opcode:0x%04X\n)",opcode);
                    break;


            

    }



}

