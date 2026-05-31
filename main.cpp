#include"chip8.h"
#include"graphics.h"
int main(){
    Chip8 chip;
    GraphicsLib graphicslib;
    while(true){
        RENDER_STATE state = chip.decode_execute();
        if(state == RENDER_STATE_RENDER){
            graphicslib.printscreen(chip.ret_display());
        }
    }
}