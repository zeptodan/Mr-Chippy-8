#include<iostream>
#include"chip8.h"
#include"graphics.h"
int main(int argc, char* argv[]){
    if (argc != 2){
        std::cout << "give rom path\n";
        return 1;
    }
    Chip8 chip;
    GraphicsLib graphicslib;
    chip.load_rom(argv[1]);
    while(true){
        RENDER_STATE state = chip.decode_execute();
        if(state == RENDER_STATE_RENDER){
            graphicslib.printscreen(chip.ret_display());
        }
    }
}