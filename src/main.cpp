#include "CHIP8.hpp"
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"           // for Event
#include <ftxui/dom/elements.hpp>  // for operator|, text, Element, hbox, bold, color, filler, separator, vbox, window, gauge, Fit, size, dim, EQUAL, WIDTH
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <bitset>

namespace ui = ftxui;
ui::Elements StrToElements(std::vector<std::string> Strings){
    ui::Elements text;
    for(const auto a : Strings){
          text.push_back(ui::text(a) | ui::flex);
    }
    return text;
}
ui::Element DisplayRegisters(std::array<unsigned char,16> Registers){
    ui::Elements elements;
    for (int i = 0; i<16; i++){
        elements.push_back(ui::text("r" + std::to_string(i) + " = " + std::bitset<8>(static_cast<long>(Registers[i])).to_string()));
    }
    return ui::vbox({elements});
}

using namespace std;
int main() {
    CHIP8 mychip;
    mychip.init("memory.txt");
    
    ftxui::Event myEvent{};
    int i{};
    vector<string> inputs;
    auto screen = ftxui::ScreenInteractive::TerminalOutput();
    auto renderer = ui::Renderer([&] {
        string character;
        if (myEvent.is_character()){
            character = myEvent.character();
            inputs.push_back(myEvent.character());
        } else {
            character = "Updating, Event was custom.";
        }
        //mychip.EmulateCycle(myEvent);
        screen.PostEvent(ui::Event::Custom);
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
        return ui::vbox({
            ui::text("Counter is: " + to_string(i++)) 
                | ui::border 
                | ui::notflex 
                | ui::color(ui::Color::MagentaLight)
            ,
            DisplayRegisters({1,2,3,4,5,6,7,8,9,0,9,8,7,6,5,4})
                
            });
    });
   
    renderer |= ftxui::CatchEvent( [&](ftxui::Event event) {
        if (event == ftxui::Event::Escape) {
            screen.ExitLoopClosure()(); 
            return true;
        } 
        myEvent = event;   
        return false;
    });
    screen.Loop(renderer);
    for(auto s:inputs){
        cout<<s<<"\n";
    }
}
