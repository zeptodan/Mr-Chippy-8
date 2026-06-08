#include"graphics.h"
GraphicsLib::GraphicsLib(){
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("Mr Chippy 8",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,CHIP_8_X * SCALE,CHIP_8_Y * SCALE,SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,CHIP_8_X / 2,CHIP_8_Y / 2);
}
GraphicsLib::~GraphicsLib(){
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}
void GraphicsLib::printscreen(uint8_t* disp,bool high_res){
    int row_len = high_res ? CHIP_8_X : CHIP_8_X / 2;
    int col_len = high_res ? CHIP_8_Y : CHIP_8_Y / 2;
    int size = row_len * col_len;
    for (int i = 0,j = 0; i < size;i++,j++){
        if (j % CHIP_8_X >= row_len){
            int row_no = j / CHIP_8_X;
            j = (row_no + 1) * CHIP_8_X;
        }
        sdl_disp[i] = colors[disp[j]];
    }
    SDL_UpdateTexture(texture,NULL,sdl_disp.data(),row_len * sizeof(uint32_t));
    SDL_Rect rect{0,0,CHIP_8_X * SCALE, CHIP_8_Y * SCALE};
    SDL_RenderCopy(renderer,texture,NULL,&rect);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
}
void GraphicsLib::setRes(bool high_res){
    SDL_DestroyTexture(texture);
    if(high_res){
        texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,CHIP_8_X,CHIP_8_Y);
    }
    else{
        texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,CHIP_8_X / 2,CHIP_8_Y / 2);
    }
}