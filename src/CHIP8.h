#include <bitset>
#include <fstream>
#include <array>
//Screen
const unsigned int SCREEN_HEIGHT = 32;
const unsigned int SCREEN_WIDTH = 64;

// Different storarge sizes
const unsigned int BYTE_LENGTH = 8;
const unsigned int WORD_LENGTH = 2*BYTE_LENGTH;
const unsigned int DWORD_LENGTH = 4*BYTE_LENGTH;
const unsigned int QWORD_LENGTH = 8*BYTE_LENGTH;
const unsigned int MEM_SIZE = 4096;
const unsigned int STACK_SIZE = 16;
using WORD = std::bitset<WORD_LENGTH>;
using BYTE = std::bitset<BYTE_LENGTH>;
using DWORD = std::bitset<DWORD_LENGTH>;
using QWORD = std::bitset<QWORD_LENGTH>;
struct CHIP8{    
    public:
    CHIP8() = default;    
    void init(std::string);
    void EmulateCycle();
    //private:
    // for if I make this x86-64, I'll use this in the register array
    union register_type{
        QWORD bit_64;
        DWORD bit_32;
        WORD bit_16;
    };
    std::array<unsigned char,MEM_SIZE> MEMORY{};
    std::array<unsigned char, 16> REGISTERS{};
    std::array<unsigned short,STACK_SIZE> STACK{};
    std::array<std::array<unsigned char, SCREEN_HEIGHT>, SCREEN_WIDTH> SCREEN{};
    std::array<unsigned char,16> KEYS;
    unsigned short OPCODE{0x0000};
    unsigned short PROGRAM_COUNTER{0x00};
    unsigned char I{0x00};
    unsigned short STACK_POINTER{0x00};

    
};
unsigned char const chip8_fontset[80] =
{ 
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
