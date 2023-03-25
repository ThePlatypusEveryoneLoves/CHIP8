#include "CHIP8.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cassert>
#include <stdlib.h>

const unsigned int FONSET_MEM_LOCATION = 0x50;
const unsigned int PROGRAM_MEM_START = 0x200;
const unsigned int CharWidth = 8;
void CHIP8::init(std::string file = "./memory.txt"){
    time_t t;
    srand(time(&t));
    PROGRAM_COUNTER = 0x200;
    OPCODE = 0;
    I = 0;
    STACK_POINTER = 0;

    for(int i = 0; i < 80; ++i){
        MEMORY[i+FONSET_MEM_LOCATION] = chip8_fontset[i];
    }
    std::ifstream ProgramReader;
    ProgramReader.open(file,std::ios_base::in);
    if(!ProgramReader){
        throw std::runtime_error("File not opened");
    }
    unsigned int input{0};
    int i{0};
    while(ProgramReader>>std::hex>>input){
        //std::cout <<PROGRAM_MEM_START+i<<std::endl;
        MEMORY[PROGRAM_MEM_START+i]=static_cast<unsigned char>(input);
        i++;
        
    }
    ProgramReader.close();
}
void CHIP8::EmulateCycle(){
    OPCODE = static_cast<int>(MEMORY[PROGRAM_COUNTER])<<8|MEMORY[PROGRAM_COUNTER+1];
    unsigned short X = ((OPCODE & 0x0F00)>>8);
    unsigned short Y = (OPCODE & 0x00F0)>>8;
     switch(OPCODE & 0xF000){
        case 0x0000:
            switch(OPCODE & 0x00FF){
                case 0x00EE:
                    { // return from subroutine
                    PROGRAM_COUNTER = STACK[STACK_POINTER];
                    --STACK_POINTER;
                    }
                    break;
                case 0x00E0:
                    {
                    SCREEN.fill({});
                    break;
                    }
            }
            break;
        case 0x1000:
            { //jmp to address
            PROGRAM_COUNTER = OPCODE & 0x0FFF;
            break;
            }
        case 0x2000: 
            {//call subroutine
            STACK[STACK_POINTER] = PROGRAM_COUNTER;
            ++STACK_POINTER;
            PROGRAM_COUNTER = OPCODE & 0x0FFF;
            break;
            }
        case 0x3000:
            {//skip next instruction if Register X is equal to BYTE
            PROGRAM_COUNTER += MEMORY[X] == (OPCODE & 0x00FF)? 2 : 0;
            break;
            }
        case 0x4000:
            { //skip next instruction if Register X is equal to BYTE
            PROGRAM_COUNTER += MEMORY[Y] != (OPCODE & 0x00FF)? 2 : 0;
            break;
            }
        case 0x5000:
            {//skip next instruction if Register X is equal to REGISTER Y
            PROGRAM_COUNTER += REGISTERS[X] == REGISTERS[Y]? 2: 0;
            break;
            }
        case 0x6000:
            { //load BYTE into register
            REGISTERS[X] = OPCODE & 0x00FF;
               
            break;
            }
        case 0x7000:{ //add register X to BYTE and store in register X
            REGISTERS[X] += OPCODE & 0x00FF;
            break;
            }
        case 0x8000:
            switch(OPCODE & 0x000F){
                case 0x0000:
                    { //load from one register to the other
                    REGISTERS[X] = REGISTERS[Y];
                    break;
                    }
                case 0x0001:{ // register X = register X OR register Y
                    REGISTERS[X] |= REGISTERS[Y];
                    break;
                    }
                case 0x0002:
                    { // register X = register X AND register Y
                    REGISTERS[X] &= REGISTERS[Y];
                    break;
                    }                
                case 0x0003:{ // register X = register X XOR register Y
                    REGISTERS[X] ^= REGISTERS[Y];
                    break;
                    }
                case 0x0004:
                    { // register X = register X + register Y, overflow bit in register F(always)
                    REGISTERS[0xF] = 0;
                    if (REGISTERS[X] + REGISTERS[Y] > 255){   
                        REGISTERS[0xF] = 1;
                    }
                    REGISTERS[X] += REGISTERS[Y];
                    break;
                    }
                case 0x0005:
                    { // register X -=  register Y, NOT(underflow bit) in register F(always)                  
                    REGISTERS[0xF] = 0;
                    if (REGISTERS[X] > REGISTERS[Y]){
                        REGISTERS[0xF] = 1;
                    } 
                    REGISTERS[X] -= REGISTERS[Y]; 
                    break;
                    }
                case 0x0006:
                    { // Register F = last bit in register X, register X is shifted one to the right
                    REGISTERS[0xF] = (X & 0x1);
                    REGISTERS[X] >>= 1;
                    break;
                    }
                case 0x0007: 
                    { // register X = register Y - register X, NOT(underflow bit) in register F(alway)                                    
                       REGISTERS[0xF] = 0;
                    if (REGISTERS[Y] > REGISTERS[X] ){
                        REGISTERS[0xF] = 1;
                    } 
                    REGISTERS[X] = REGISTERS[Y]-REGISTERS[X];
                    break;
                    }
                case 0x000E:
                    {// Register F = first bit in register X, register X is shifted one to the left
                    REGISTERS[0xF] = (X & 0x80);
                    REGISTERS[X] <<= 1;
                    break;
                    }
            }
            break;
        case 0x9000:
            { //skip next instuction if REGISTER X != REGISTER Y
             if(REGISTERS[X] != REGISTERS[Y]){
                 PROGRAM_COUNTER += 2;
             }
             break;
            }
        case 0xA000:
            { //load nnn into I
            
             I = (OPCODE & 0x0FFF);
             break;
            }
        case 0xB000:
            {
             PROGRAM_COUNTER = I + (OPCODE & 0x0FFF);
                break;
            }
        case 0xC000: 
             {// random number ANDed with BYTE
             REGISTERS[X] = ((rand()%0x100) & (OPCODE & 0x00FF));
             break;
             }
        case 0xD000:
            {
             unsigned char height = OPCODE & 0xF;
             REGISTERS[0xF]  = 0;
             for(int j{0};j < height;j++){
                unsigned char sprite = MEMORY[I+j];/*
                for(int k{0};k<CharWidth;k++){
                        if(SCREEN[REGISTERS[Y]+j%SCREEN_WIDTH][REGISTERS[X]+k%SCREEN_HEIGHT]){
                            REGISTERS[0xF] = 1;
                        }
                        SCREEN[REGISTERS[Y]+j%SCREEN_WIDTH][REGISTERS[X]+k%SCREEN_HEIGHT] ^= 1; 
                    }*/
                }     
             
             break;
            }
        case 0xE000:
            
             break;
        case 0xF000:
            //switch(){}
            break;
            
        
    }
    PROGRAM_COUNTER += 2;
    std::cout<<std::hex<<OPCODE<<std::endl;
}
/*
    first # bigger
     101010
    +011100
    =======
    1000110
    ^ 
  ignored(or put in another register, usually a flag)
    second # bigger
     100100
    +010110
    =======
    0111010
    

*/
