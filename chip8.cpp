#include<fstream>
#include<iostream>
#include"chip8.h"
Chip8::Chip8 () {
    std::copy(fonts.begin(),fonts.end(),ram.begin());
    std::copy(big_fonts.begin(),big_fonts.end(),ram.begin() + BIG_FONT_START);
    std::fill(sound_buffer.begin(), sound_buffer.begin() + (SOUND_BUFFER_SIZE / 2), 1);
    std::fill(sound_buffer.begin() + (SOUND_BUFFER_SIZE / 2), sound_buffer.end(), 0);
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
double Chip8::get_pitch() const {
    return pitch;
}
std::array<uint8_t,SOUND_BUFFER_SIZE>& Chip8::get_sound_buffer(){
    return sound_buffer;
}
bool Chip8::has_resChanged(){
    return has_res_changed;
}
void Chip8::set_has_resChanged(bool has){
    has_res_changed = has;
}
uint8_t Chip8::get_plane(){
    return plane;
}
void Chip8::draw_sprite(uint8_t VX, uint8_t VY, uint8_t N, uint8_t plane, uint16_t index){
    int row_len = quirks.high_res ? CHIP_8_X : CHIP_8_X / 2;
    int col_len = quirks.high_res ? CHIP_8_Y : CHIP_8_Y / 2;
    VX = VX % row_len;
    VY = VY % col_len;
    uint16_t pixels;
    uint16_t rows = N;
    uint8_t tot_bits = 8;
    if (N == 0){
        rows = 16;
        tot_bits = 16;
    }
    for (int row = 0; row < rows;row++){
        if (N == 0){
            pixels = (ram[index + 2 * row] << 8) | ram[index + 2 * row + 1];
        }
        else{
            pixels = ram[index + row];
        }
        for (int bit = 0;bit < tot_bits;bit++){
            int pos = VX + CHIP_8_X * (VY + row) + bit;
            if (VX + bit >= row_len){
                continue;
            }
            if (VY + row >= col_len){
                continue;
            }
            uint8_t pixel = (pixels >> (tot_bits - 1 - bit)) & 1;
            if (plane == 2){
                pixel <<= 1;
            }
            if ((disp[pos] & plane) && pixel){
                regs[0xF] = 1;
            }
            disp[pos] ^= pixel;
        }
    }
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
                for (std::size_t i = 0; i < disp.size(); i++){
                    disp[i] &= ~plane;
                }
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
                            disp[i + j * CHIP_8_X + 4] = (disp[i + j * CHIP_8_X + 4] & (~plane)) | (disp[i + j * CHIP_8_X] & plane);
                        }
                    }
                    for(int i = 0; i < 4;i++){
                        for(int j = 0;j < rows;j++){
                            disp[i + j * CHIP_8_X] &= ~plane;
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
                            disp[i + j * CHIP_8_X - 4] = (disp[i + j * CHIP_8_X - 4] & (~plane)) | (disp[i + j * CHIP_8_X] & plane);
                        }
                    }
                    for(int i = cols - 1; i > cols - 5;i--){
                        for(int j = 0;j < rows;j++){
                            disp[i + j * CHIP_8_X] &= ~plane;
                        }
                    }
                }
                    break;
                case 0x00FD:
                    std::cout << "Exiting Emulator" << std::endl;
                    exit(1);
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
                                disp[j + (i + N) * CHIP_8_X] = (disp[j + (i + N) * CHIP_8_X] & (~plane)) | (disp[j + i * CHIP_8_X] & plane);
                            }
                        }
                        for(int i = 0;i < N ;i++){
                            for(int j = 0;j < cols;j++){
                                disp[j + i * CHIP_8_X] &= ~plane;
                            }
                        }
                    }
                    else if ((op & 0x0FF0) == 0x00D0){
                        int cols = quirks.high_res ? CHIP_8_X : CHIP_8_X / 2;
                        int rows = quirks.high_res ? CHIP_8_Y : CHIP_8_Y / 2;
                        for(int i = N;i < rows ;i++){
                            for(int j = 0;j < cols;j++){
                                disp[j + (i - N) * CHIP_8_X] = (disp[j + (i - N) * CHIP_8_X] & (~plane)) | (disp[j + i * CHIP_8_X] & plane);
                            }
                        }
                        for(int i = rows - 1;i > rows - N - 1 ;i--){
                            for(int j = 0;j < cols;j++){
                                disp[j + i * CHIP_8_X] &= ~plane;
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
                if(fetch() == 0xF000){
                    pc += 2;
                }
            }
            break;
        case 0x4000:
            if (regs[X] != NN){
                if(fetch() == 0xF000){
                    pc += 2;
                }
            }
            break;
        case 0x5000:
            switch (op & 0x000F)
            {
            case 0x0000:
                if (regs[X] == regs[Y]){
                    if(fetch() == 0xF000){
                        pc += 2;
                    }
                }
                break;
            case 0x0002:
                for (int i = X, m = 0, step = (X <= Y) ? 1 : -1 ;;i += step, m++){
                    ram[index + m] = regs[i];
                    if (i == Y)
                        break;
                }
                break;
            case 0x0003:
                for (int i = X, m = 0, step = (X <= Y) ? 1 : -1 ;;i += step, m++){
                    regs[i] = ram[index + m];
                    if (i == Y)
                        break;
                }
                break;
            default:
                std::cout << "unrecognized op " << std::hex <<  op << std::dec << std::endl;
                exit(1);
                break;
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
                    //regs[0xF] = 0;
                    break;
                case 0x0002:
                    regs[X] &= regs[Y];
                    //regs[0xF] = 0;
                    break;
                case 0x0003:
                    regs[X] ^= regs[Y];
                    //regs[0xF] = 0;
                    break;
                case 0x0004:
                {
                    uint8_t flag;
                    if (regs[X] + regs[Y] > 255){
                        flag = 1;
                    }
                    else{
                        flag = 0;
                    }
                    regs[X] += regs[Y];
                    regs[0xF] = flag;
                }
                    break;
                case 0x0005:
                {
                    uint8_t flag;
                    if (regs[X]  < regs[Y]){
                        flag = 0;
                    }
                    else {
                        flag = 1;
                    }
                    regs[X] -= regs[Y];
                    regs[0xF] = flag;
                }
                    break;
                case 0x0006:
                {
                    regs[X] = regs[Y];
                    uint8_t flag = regs[X] & 0x01;
                    regs[X] >>= 1;
                    regs[0xF] = flag;
                }
                    break;
                case 0x0007:
                {
                    uint8_t flag;
                    if (regs[Y] < regs[X]){
                        flag = 0;
                    }
                    else {
                        flag = 1;
                    }
                    regs[X] = regs[Y] - regs[X];
                    regs[0xF] = flag;
                }
                    break;
                case 0x000E:
                {
                    regs[X] = regs[Y];
                    uint8_t flag = (regs[X] >> 7) & 0x01;
                    regs[X] <<= 1;
                    regs[0xF] = flag;
                }
                    break;
                default:
                    std::cout << "unrecognized op " << std::hex <<  op << std::dec << std::endl;
                    exit(1);
                    break;
            }
            break;
        case 0x9000:
            if (regs[X] != regs[Y]){
                if(fetch() == 0xF000){
                    pc += 2;
                }
            }
            break;
        case 0xA000:
            index = NNN;
            break;
        case 0xB000:
            pc = NNN + regs[0x0];
            break;
        case 0xC000:
            regs[X] = (rng() & 0xFF) & NN;
            break;
        case 0xD000:
            {
                uint8_t temp = 0;
                uint8_t VX = regs[X], VY = regs[Y];
                regs[0xF] = 0;
                if (plane & 0x1){
                    draw_sprite(VX,VY,N,plane & 0x1,index);
                    temp = N ? N : 32;
                }
                if (plane & 0x2){
                    draw_sprite(VX,VY,N,plane & 0x2,index + temp);
                }
            }
            return RENDER_STATE_RENDER;
            break;
        case 0xE000:
            switch (op & 0x00FF)
            {
            case 0x009E:
                if (keys[regs[X]]){
                    if(fetch() == 0xF000){
                        pc += 2;
                    }
                }
                break;
            case 0x00A1:
                if (!keys[regs[X]]){
                    if(fetch() == 0xF000){
                        pc += 2;
                    }
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
                case 0x0000:
                    index = fetch();
                    break;
                case 0x0001:
                    plane = X;
                    break;
                case 0x0002:
                {
                    int sound_index = 0;
                    for (int i = index; i < index + 16;i++){
                        uint8_t byte = ram[i];
                        for (int bit = 0;bit < 8; bit++){
                            uint8_t sound_bit = (byte >> (7 - bit)) & 0x1;
                            sound_buffer[sound_index++] = sound_bit;
                        }
                    }
                }
                    break;
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
                {
                    uint8_t flag = ((index + regs[X]) > 0x0FFF);
                    index += regs[X];
                    regs[0xF] = flag;
                }
                    break;
                case 0x0029:
                    index = regs[X] * 5;
                    break;
                case 0x0030:
                    index = BIG_FONT_START + regs[X] * 10;
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
                case 0x003A:
                    pitch = 4000.0 * std::pow(2.0, (regs[X] - 64.0) / 48.0);
                    break;
                case 0x0055:
                    for (int i = 0; i <= X; i++){
                        ram[index + i] = regs[i]; 
                    }
                    index += X + 1;
                    break;
                case 0x0065:
                    for (int i = 0; i <= X; i++){
                        regs[i] = ram[index + i]; 
                    }
                    index += X + 1;
                    break;
                case 0x0075:
                    for (int i = 0; i <= X; i++){
                        rpl[i] = regs[i]; 
                    }
                    break;
                case 0x0085:
                    for (int i = 0; i <= X; i++){
                        regs[i] = rpl[i]; 
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