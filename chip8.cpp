#include<fstream>
#include<iostream>
#include"chip8.h"
Chip8::Chip8 () {
    std::copy(ram.begin(),ram.begin() + fonts.size(),fonts.begin());
}
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
    uint16_t X = (op & 0x0F00) >> 8;
    uint16_t Y = (op & 0x00F0) >> 4;
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
        case 0x8000:
            switch (op & 0x000F)
            {
                case 0x0000:
                    regs[X] = regs[Y];
                    break;
                case 0x0001:
                    regs[X] |= regs[Y];
                    break;
                case 0x0002:
                    regs[X] &= regs[Y];
                    break;
                case 0x0003:
                    regs[X] ^= regs[Y];
                    break;
                case 0x0004:
                    regs[X] += regs[Y];
                    if (regs[X] + regs[Y] > 255){
                        regs[0xF] = 1;
                    }
                    else{
                        regs[0xF] = 0;
                    }
                    break;
                case 0x0005:
                    regs[X] -= regs[Y];
                    if (regs[X] - regs[Y] <0){
                        regs[0xF] = 0;
                    }
                    else {
                        regs[0xF] = 1;
                    }
                    break;
                case 0x0006:
                    regs[X] = regs[Y];
                    regs[0xF] = regs[X] & 0x01;
                    regs[X] >>= 1;
                    break;
                case 0x0007:
                    regs[X] = regs[Y] - regs[X];
                    if (regs[Y] - regs[X] <0){
                        regs[0xF] = 0;
                    }
                    else {
                        regs[0xF] = 1;
                    }
                    break;
                case 0x000E:
                    regs[X] = regs[Y];
                    regs[0xF] = (regs[X] >> 7) & 0x01;
                    regs[X] <<= 1;
                    break;
                default:
                    std::cout << "unrecognized op " << std::hex <<  op << std::dec << std::endl;
                    exit(1);
                    break;
            }
            break;
        case 0x9000:
            if (regs[X] != regs[Y]){
                pc += 2;
            }
            break;
        case 0xA000:
            index = NNN;
            break;
        case 0xB000:
            pc = NNN + regs[0x0];
            break;
        case 0xC000:
            regs[X] = (rand() % NN) & NN;
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
        case 0xE000:
            switch (op & 0x00FF)
            {
            case 0x009E:
                if (keys[regs[X]]){
                    pc+=2;
                }
                break;
            case 0x00A1:
                if (!keys[regs[X]]){
                    pc+=2;
                }
                break;
            default:
                std::cout << "unrecognized op " << std::hex <<  op << std::dec << std::endl;
                exit(1);
                break;
            }
            break;
        case 0xF000:
            switch (op & 0x00FF)
            {
                case 0x0007:
                    regs[X] = d_time;
                    break;
                case 0x000A:
                    for (int i = 0;i < TOT_KEYS;i++){
                        if (keys[i]){
                            regs[X] = i;
                        }
                        else if (i == TOT_KEYS - 1){
                            pc -= 2;
                        }
                    }
                    break;
                case 0x0015:
                    d_time = regs[X];
                    break;
                case 0x0018:
                    s_time = regs[X];
                    break;
                case 0x001E:
                    if (!(index & 0xF000) && ((index + regs[X]) & 0xF000)){
                        regs[0xF] = 1;
                    }
                    else {
                        regs[0xF] = 0;
                    }
                    index += regs[X];
                    break;
                case 0x0029:
                    index = ram[regs[X] * 5];
                    break;
                case 0x0033:
                {
                    uint8_t VX = regs[X];
                    ram[index + 2] = VX % 10;
                    VX /= 10;
                    ram[index + 1] = VX % 10;
                    VX /= 10;
                    ram[index] = VX % 10;
                }
                    break;
                case 0x0055:
                    for (int i = 0; i < X; i++){
                        ram[index + i] = regs[i]; 
                    }
                    break;
                case 0x0065:
                    for (int i = 0; i < X; i++){
                        regs[i] = ram[index + i]; 
                    }
                    break;
                default:
                    std::cout << "unrecognized op " << std::hex <<  op << std::dec << std::endl;
                    exit(1);
                    break;
            }
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
Keys& Chip8::ret_keys(){
    return keys;
}
bool Chip8::update_timers(){
    if (d_time != 0)
        d_time--;
    if (s_time != 0)
        s_time--;
    return s_time == 0 ? false : true;
}