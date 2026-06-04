#include<fstream>
#include<iostream>
#include"chip8.h"
Chip8::Chip8 () {
    std::copy(fonts.begin(),fonts.end(),ram.begin());
    std::copy(big_fonts.begin(),big_fonts.end(),ram.begin() + BIG_FONT_START);
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
const Quirks& Chip8::getQuirks() const {
    return quirks;
}
bool Chip8::has_resChanged(){
    return has_res_changed;
}
void Chip8::set_has_resChanged(bool has){
    has_res_changed = has;
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
            switch (op)
            {
                case 0x00E0:
                    disp.fill(0);
                    return RENDER_STATE_RENDER;
                    break;
                case 0x00EE:
                    pc = stack.top();
                    stack.pop();
                    break;
                case 0x00FB:
                {
                    int cols = quirks.high_res ? CHIP_8_X : CHIP_8_X / 2;
                    int rows = quirks.high_res ? CHIP_8_Y : CHIP_8_Y / 2;
                    for(int i = cols - 5;i >=0;i--){
                        for(int j = 0;j < rows;j++){
                            disp[i + j * CHIP_8_X + 4] = disp[i + j * CHIP_8_X];
                        }
                    }
                    for(int i = 0; i < 4;i++){
                        for(int j = 0;j < rows;j++){
                            disp[i + j * CHIP_8_X] = 0;
                        }
                    }
                }
                    break;
                case 0x00FC:
                {
                    int cols = quirks.high_res ? CHIP_8_X : CHIP_8_X / 2;
                    int rows = quirks.high_res ? CHIP_8_Y : CHIP_8_Y / 2;
                    for(int i = 4;i < cols;i++){
                        for(int j = 0;j < rows;j++){
                            disp[i + j * CHIP_8_X - 4] = disp[i + j * CHIP_8_X];
                        }
                    }
                    for(int i = cols - 1; i >= cols - 5;i--){
                        for(int j = 0;j < rows;j++){
                            disp[i + j * CHIP_8_X] = 0;
                        }
                    }
                }
                    break;
                case 0x00FE:
                    quirks.high_res = false;
                    has_res_changed = true;
                    break;
                case 0x00FF:
                    quirks.high_res = true;
                    has_res_changed = true;
                    break;
                default:
                    if ((op & 0x0FF0) == 0x00C0){
                        int cols = quirks.high_res ? CHIP_8_X : CHIP_8_X / 2;
                        int rows = quirks.high_res ? CHIP_8_Y : CHIP_8_Y / 2;
                        for(int i = rows - N - 1;i >= 0 ;i--){
                            for(int j = 0;j < cols;j++){
                                disp[j + (i + N) * CHIP_8_X] = disp[j + i * CHIP_8_X];
                            }
                        }
                        for(int i = 0;i < N ;i++){
                            for(int j = 0;j < cols;j++){
                                disp[j + i * CHIP_8_X] = 0;
                            }
                        }
                    }
                    else{
                        std::cout << "unrecognized op " << std::hex <<  op << std::dec << std::endl;
                        exit(1);
                    }
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
                    if (regs[X] + regs[Y] > 255){
                        regs[0xF] = 1;
                    }
                    else{
                        regs[0xF] = 0;
                    }
                    regs[X] += regs[Y];
                    break;
                case 0x0005:
                    if (regs[X] - regs[Y] <0){
                        regs[0xF] = 0;
                    }
                    else {
                        regs[0xF] = 1;
                    }
                    regs[X] -= regs[Y];
                    break;
                case 0x0006:
                    //regs[X] = regs[Y];
                    regs[0xF] = regs[X] & 0x01;
                    regs[X] >>= 1;
                    break;
                case 0x0007:
                    if (regs[Y] - regs[X] <0){
                        regs[0xF] = 0;
                    }
                    else {
                        regs[0xF] = 1;
                    }
                    regs[X] = regs[Y] - regs[X];
                    break;
                case 0x000E:
                    //regs[X] = regs[Y];
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
                int row_len = quirks.high_res ? CHIP_8_X : CHIP_8_X / 2;
                int col_len = quirks.high_res ? CHIP_8_Y : CHIP_8_Y / 2;
                uint8_t VX = regs[X] % row_len;
                uint8_t VY = regs[Y] % col_len;
                for (int row = 0;row < N;row++){
                    uint8_t pixels = ram[index + row];
                    for(int bit = 0;bit < 8;bit++){
                        int pos = VX + CHIP_8_X * (VY + row) + bit;
                        if (VX + bit >= CHIP_8_X){
                            continue;
                        }
                        if (VY + row >= CHIP_8_Y){
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
                            break;
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
                    index = regs[X] * 5;
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