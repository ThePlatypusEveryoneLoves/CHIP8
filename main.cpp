#include <ios>
#include <iostream>
#include <array>
#include <cassert>
#include <fstream>
#include "CHIP8.hpp"
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, vbox, window, Element, Elements
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"           // for Event
void (*pf[])(void){[](){int x = 1; std::cout<<x<<std::endl;return;},[](){int x = 2;  std::cout<<x<<std::endl;return;}};
std::array<void(*)(), 4> l{[](){int x = 1; std::cout<<x<<std::endl;return;},[](){int x = 2;  std::cout<<x<<std::endl;return;}};

using namespace std;
int main() {
   CHIP8 mychip;
    //int *prt = new int;
    
    mychip.init("memory.txt");
    assert(mychip.MEMORY.size()==4096);
    unsigned int a;
    std::fstream f("memory.txt");
    int i = 0;
    while ( f >>std::hex>> a) {
        assert(a==static_cast<int>(mychip.MEMORY[0x200+i]));
        i++;
        
    }
    
    while (mychip.PROGRAM_COUNTER <=0x250) {
        mychip.EmulateCycle();
    }
    
    std::cout<<std::dec<<mychip.STACK_POINTER<<"\n";
    //std::cout<<std::hex<<static_cast<unsigned int>(mychip.MEMORY[0x205])<<std::endl;

}
