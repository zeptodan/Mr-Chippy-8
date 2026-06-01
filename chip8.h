#include<cstdint>
#include<array>
#include<stack>
#include<string>
#include"constants.h"
enum RENDER_STATE {
    RENDER_STATE_RENDER,
    RENDER_STATE_NOT_RENDER
};
using Keys = std::array<bool,TOT_KEYS>;
class Chip8 {
    private:
    std::array<uint8_t,CHIP_8_X * CHIP_8_Y> disp{};
    Keys keys{};
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
    RENDER_STATE decode_execute();
    uint8_t* ret_display();
    void check_input();
    Keys& ret_keys();
};