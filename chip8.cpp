#include<fstream>
#include"chip8.h"
void Chip8::load_rom(std::string file_path){
    std::ifstream file(file_path, std::ios::binary);
    file.read(reinterpret_cast<char*>(ram.data() + ROM_START),ram.size() - ROM_START);
    return;
}
Chip8::Chip8() : regs{}, ram{}, index{}, pc{}, d_time{},s_time{} {}
uint16_t Chip8::fetch(){
    uint16_t op =  (ram[pc] >> 8) | ram[pc + 1];
    return op;
}
void Chip8::decode_execute(){
    uint16_t op = fetch();
    uint16_t X = op | 0x0F00;
    uint16_t Y = op | 0x00F0;
    uint16_t N = op | 0x000F;
    uint16_t NN = op | 0x00FF;
    uint16_t NNN = op | 0x0FFF;
    switch (op | 0xF000)
    {
        case 0x0000:
            break;
        case 0x1000:
            break;
        case 0x6000:
            break;
        case 0x7000:
            break;
        case 0xA000:
            break;
        case 0xD000:
            break;
        default:
            break;
    }

}