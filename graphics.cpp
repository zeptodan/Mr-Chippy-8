#include"graphics.h"
GraphicsLib::GraphicsLib(){
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("CHIP-8",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,CHIP_8_X * SCALE,CHIP_8_Y * SCALE,SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,CHIP_8_X,CHIP_8_Y);
}
GraphicsLib::~GraphicsLib(){
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}
void GraphicsLib::printscreen(uint8_t* disp){
    for (std::size_t i = 0; i < sdl_disp.size();i++){
        sdl_disp[i] = (disp[i] == 0 ? 0x000000FF : 0xFFFFFFFF);
    }
    SDL_UpdateTexture(texture,NULL,sdl_disp.data(),CHIP_8_X * sizeof(uint32_t));
    SDL_Rect rect{0,0,CHIP_8_X * SCALE, CHIP_8_Y * SCALE};
    SDL_RenderCopy(renderer,texture,NULL,&rect);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
}