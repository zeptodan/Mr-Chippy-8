#include<SDL2/SDL.h>
#include<array>
#define CHIP_8_X 64
#define CHIP_8_Y 32
#define SCALE 15
class GraphicsLib {
    private:
    std::array<uint32_t,CHIP_8_X * CHIP_8_Y> disp{};
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    public:
    GraphicsLib();
    ~GraphicsLib();
    void printscreen();
    void clearscreen();
    uint8_t displaysprite(uint8_t index, uint8_t x, uint8_t y, uint8_t n);
};