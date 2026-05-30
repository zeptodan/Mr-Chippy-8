#include<cstdint>
#include<array>
#include<stack>
#include<string>
#define RAM_SIZE 4096
#define TOT_REGS 16
class Chip8 {
    std::array<int8_t,RAM_SIZE> ram[RAM_SIZE];
    std::array<int8_t,TOT_REGS> regs[TOT_REGS];
    std::stack<int16_t> stack;
    int16_t pc;
    int16_t index;
    int8_t d_time;
    int8_t s_time;
    void load_rom(std::string file_path);
    
};