#include<SDL2/SDL.h>
#include<array>
#include"constants.h"
class GraphicsLib {
    private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    std::array<uint32_t,CHIP_8_X * CHIP_8_Y> sdl_disp{};
    public:
    GraphicsLib();
    ~GraphicsLib();
    void printscreen(uint8_t*);
};