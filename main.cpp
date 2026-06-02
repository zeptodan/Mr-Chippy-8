#include<iostream>
#include"chip8.h"
#include"graphics.h"
void process_event(Keys& keys, SDL_Event& ev){
    switch (ev.type)
    {
    case SDL_QUIT:
        exit(0);
        break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        switch (ev.key.keysym.scancode)
        {
        case SDLK_1:keys[0x0] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDLK_2:keys[0x1] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDLK_3:keys[0x2] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDLK_4:keys[0x3] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDLK_q:keys[0x4] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDLK_w:keys[0x5] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDLK_e:keys[0x6] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDLK_r:keys[0x7] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDLK_a:keys[0x8] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDLK_s:keys[0x9] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDLK_d:keys[0xA] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDLK_f:keys[0xB] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDLK_z:keys[0xC] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDLK_x:keys[0xD] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDLK_c:keys[0xE] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDLK_v:keys[0xF] = ev.type == SDL_KEYDOWN ? true: false;break;
        default:
            break;
        }
    default:
        break;
    }
}
void play_sound(bool ring){

}
int main(int argc, char* argv[]){
    if (argc != 2){
        std::cout << "give rom path\n";
        return 1;
    }
    Chip8 chip;
    GraphicsLib graphicslib;
    chip.load_rom(argv[1]);
    SDL_Event ev;
    bool ring = false;
    uint64_t start, end, ms_per_frame = 1000 / FPS;
    while(true){
        start = SDL_GetTicks64();
        if (SDL_PollEvent(&ev) != 0){
            process_event(chip.ret_keys(),ev);
        }
        for (int i = 0; i < INSTRUCTION_RATE; i++){
            chip.decode_execute();
        }
        graphicslib.printscreen(chip.ret_display());
        end = SDL_GetTicks64();
        if (end - start < ms_per_frame){
            SDL_Delay(ms_per_frame - (end - start));
        }
        ring = chip.update_timers();
        play_sound(ring);
    }
}
