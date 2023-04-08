

#include "CHIP8.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cassert>
#include <stdlib.h>
#include <cmath>
#include "ftxui/component/event.hpp"           // for Event

using ScreenArray = std::array<std::array<unsigned char, SCREEN_HEIGHT>, SCREEN_WIDTH>;
const unsigned int FONSET_MEM_LOCATION = 0x50;
const unsigned int PROGRAM_MEM_START = 0x200;
const unsigned int CharWidth = 8;

void CHIP8::init(std::string file = "./memory.txt"){
    
    time_t t;
    srand(time(&t));
    //set up all of the variables
    PROGRAM_COUNTER = PROGRAM_MEM_START;
    OPCODE = 0;
    I = 0;
    STACK_POINTER = 0;
    SoundTimer = 60;
    DelayTimer = 60;
    //load fontset
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
    //load program
    while(ProgramReader>>std::hex>>input){
        MEMORY[PROGRAM_MEM_START+i]=static_cast<unsigned char>(input);
        i++;
        
    }
    ProgramReader.close();
}
void CHIP8::EmulateCycle(ftxui::Event CurrEvent){
    OPCODE = (MEMORY[PROGRAM_COUNTER])<<8|MEMORY[PROGRAM_COUNTER+1]; 
    unsigned short X = ((OPCODE & 0x0F00)>>8);
    unsigned short Y = ((OPCODE & 0x00F0)>>8);
    DelayTimer = DelayTimer > 1? DelayTimer-1 : 60;
    SoundTimer = SoundTimer > 1? SoundTimer-1 : 60;

    // the giant switch statement of OPCODES
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
                    { // reset screen
                    SCREEN.fill({});
                    break;
                    }
            }
            break;
        case 0x1000:
            { //jump to address
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
        case 0x7000:
            { //add register X to BYTE and store in register X
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
                case 0x0001:
                    { // register X = register X OR register Y
                    REGISTERS[X] |= REGISTERS[Y];
                    break;
                    }
                case 0x0002:
                    { // register X = register X AND register Y
                    REGISTERS[X] &= REGISTERS[Y];
                    break;
                    }                
                case 0x0003:
                    { // register X = register X XOR register Y
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
                    { // register X = register Y - register X, NOT(underflow bit) in register F(always)
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
            {//program counter = REGISTER 0 + nnn
             PROGRAM_COUNTER = REGISTERS[0x0] + (OPCODE & 0x0FFF);
                break;
            }
        case 0xC000: 
             {// random number ANDed with BYTE
             REGISTERS[X] = ((rand()%0x100) & (OPCODE & 0x00FF));
             break;
             }
        case 0xD000:
            {// display n-byte sprite starting at memory location I at (REGISTER X, REGISTER Y), set REGISTER 0xF = collision.
             unsigned char height = OPCODE & 0xF;
             REGISTERS[0xF]  = 0;
             for(int j{0};j < height;j++){
                unsigned char sprite = MEMORY[I+j];
                for(int k{0};k<CharWidth;k++){
                    if((sprite & 0x80)>0){
                        if(SCREEN[REGISTERS[Y]+k%SCREEN_HEIGHT][REGISTERS[X]+j%SCREEN_WIDTH]){
                            REGISTERS[0xF] = 1;
                        }
                         SCREEN[REGISTERS[Y]+k%SCREEN_HEIGHT][REGISTERS[X]+j%SCREEN_WIDTH] ^= 1;
                       
                    }
                }     
             }
             break;
            }
        case 0xE000:
            {
                switch(OPCODE & 0x00FF){
                    case 0x9E:
                    {//skip next instruction if character pressed is in REGISTER X
                        if (CurrEvent.is_character()&&(std::string(REGISTERS[X],1) == CurrEvent.character())){
                            PROGRAM_COUNTER+=2;
                        } 
                        break;
                    }
                    case 0xA1:
                    {//skip next instruction if character pressed is not in REGISTER X
                        if (CurrEvent.is_character()&&!(std::string(REGISTERS[X],1) == CurrEvent.character())){
                            PROGRAM_COUNTER += 2;
                        }
                        break;
                    }
                break;
            }
        }
        case 0xF000:
            switch(OPCODE & 0x00FF){
                case 0x07:
                { //set REGISTER X to the value of the delay timer
                    REGISTERS[X] = DelayTimer;
                    break;
                }
                case 0x0A:
                { //wait until next character, then store it in REGISTER X
                    if(!CurrEvent.is_character()){
                        return;
                    }
                    REGISTERS[X] = CurrEvent.character()[0];
                    break;
                }
                case 0x15:
                { //set delay timer to the value in REGISTER X
                    DelayTimer = REGISTERS[X];
                    break;
                }
                case 0x18:
                { // set sound timer to the value in REGISTER X 
                    SoundTimer = REGISTERS[X];
                    break;
                }
                case 0x1E:
                { //add REGISTER X to I
                    I += REGISTERS[X];
                    break;
                }
                case 0x29:
                { // set location of I to the hexadecimal sprite in REGISTER X
                   I = FONSET_MEM_LOCATION + (REGISTERS[X]*5);
                }
                case 0x33:
                { //store decimal representation of REGISTER X in I (hundredths place), I+1 (tenths place) and I+2 (ones place)
                    MEMORY[I] = floor(REGISTERS[X]/100);
                    MEMORY[I+1] = floor((REGISTERS[X]%100)/10);
                    MEMORY[I+2] = (REGISTERS[X]%10);
                    break;
                }
                case 0x55:
                {//Set MEMORY I to MEMORY I+X to REGISTER 0 to REGISTER X
                    for(int j{0};j<X;j++){
                        MEMORY[j+I] = REGISTERS[j];
                    }
                    break;
                }
                case 0x65:
                {//Set REGISTER 0 to REGISTER X to MEMORY I to MEMORY I+X
                    for(int j{0};j<X;j++){
                        REGISTERS[j] = MEMORY[j+I];
                    }
                    break;
                }
            }
            break;
    }
    PROGRAM_COUNTER += 2;
}
/*
    first # bigger
     01010 -> 10
    +11000 -> -8
    ======
     10010 -> 2
     ^ 
    ignored(or put in another register, usually a flag)
    second # bigger
     01000 -> 8
    +10110 -> -10
    =======
     01101 -> -2
     00010 -> 2
    

*/
ScreenArray CHIP8::DumpScreen(){
    return SCREEN;
}
std::array<unsigned char,MEM_SIZE> CHIP8::DumpMem(){
    return MEMORY;
}
std::array<unsigned char,REGISTER_SIZE> CHIP8::DumpRegisters(){
    return REGISTERS;
}
std::array<unsigned short, STACK_SIZE> CHIP8::DumpStack(){
    return STACK;
}
