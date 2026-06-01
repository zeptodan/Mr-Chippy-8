#include<fstream>
#include<iostream>
#include"chip8.h"
void Chip8::load_rom(std::string file_path){
    std::ifstream file(file_path, std::ios::binary);
    file.read(reinterpret_cast<char*>(ram.data() + ROM_START),ram.size() - ROM_START);
    pc = ROM_START;
    return;
}
uint16_t Chip8::fetch(){
    uint16_t op =  (ram[pc] << 8) | ram[pc + 1];
    pc +=2;
    return op;
}
RENDER_STATE Chip8::decode_execute(){
    uint16_t op = fetch();
    uint16_t X = op & 0x0F00;
    uint16_t Y = op & 0x00F0;
    uint16_t N = op & 0x000F;
    uint16_t NN = op & 0x00FF;
    uint16_t NNN = op & 0x0FFF;
    switch (op & 0xF000)
    {
        case 0x0000:
            switch (op & 0x000F)
            {
            case 0x0000:
                disp.fill(0);
                return RENDER_STATE_RENDER;
                break;
            case 0x000E:
                pc = stack.top();
                stack.pop();
                break;
            default:
                std::cout << "unrecognized op " << std::hex <<  op << std::dec << std::endl;
                exit(1);
                break;
            }
            break;
        case 0x1000:
            pc = NNN;
            break;
        case 0x2000:
            stack.push(pc);
            pc = NNN;
            break;
        case 0x3000:
            if (regs[X] == NN){
                pc += 2;
            }
            break;
        case 0x4000:
            if (regs[X] != NN){
                pc += 2;
            }
            break;
        case 0x5000:
            if (regs[X] == regs[Y]){
                pc += 2;
            }
            break;
        case 0x6000:
            regs[X] = NN;
            break;
        case 0x7000:
            regs[X] += NN;
            break;
        case 0x9000:
            if (regs[X] != regs[Y]){
                pc += 2;
            }
            break;
        case 0xA000:
            index = NNN;
            break;
        case 0xD000:
            regs[0xF] = 0;
            {
                uint8_t VX = regs[X] % CHIP_8_X;
                uint8_t VY = regs[Y] % CHIP_8_Y;
                for (int row = 0;row < N;row++){
                    uint8_t pixels = ram[index + row];
                    for(int bit = 0;bit < 8;bit++){
                        int pos = VX + CHIP_8_X * (VY + row) + bit;
                        if (VX + bit >= CHIP_8_X){
                            continue;
                        }
                        uint8_t pixel = (pixels >> (7 - bit)) & 1;
                        if (disp[pos] && pixel){
                            regs[0xF] = 1;
                        }
                        disp[pos] ^= pixel;
                    }
                }
            }
            return RENDER_STATE_RENDER;
            break;
        default:
            std::cout << "unrecognized op " << std::hex <<  op << std::dec << std::endl;
            exit(1);
            break;
    }
    return RENDER_STATE_NOT_RENDER;
}
uint8_t* Chip8::ret_display(){
    return disp.data();
}