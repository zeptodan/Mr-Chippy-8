#include<cstdint>
#include<array>
#include<stack>
#include<string>
#include"graphics.h"
#define RAM_SIZE 4096
#define TOT_REGS 16
#define ROM_START 512
class Chip8 {
    private:
    GraphicsLib graphicslib;
    std::array<uint8_t,RAM_SIZE> ram{};
    std::array<uint8_t,TOT_REGS> regs{};
    std::stack<uint16_t> stack;
    uint16_t pc{};
    uint16_t index{};
    uint8_t d_time{};
    uint8_t s_time{};
    public:
    void load_rom(std::string file_path);
    uint16_t fetch();
    void decode_execute();
};