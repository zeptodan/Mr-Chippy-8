#include<iostream>
#include"chip8.h"
#include"graphics.h"
void process_event(Keys& keys, SDL_Event& ev){
    switch (ev.type)
    {
    case SDL_QUIT:
        exit(0);
        break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        switch (ev.key.keysym.scancode)
        {
        case SDL_SCANCODE_1:keys[0x1] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDL_SCANCODE_2:keys[0x2] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDL_SCANCODE_3:keys[0x3] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDL_SCANCODE_4:keys[0xC] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDL_SCANCODE_Q:keys[0x4] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDL_SCANCODE_W:keys[0x5] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDL_SCANCODE_E:keys[0x6] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDL_SCANCODE_R:keys[0xD] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDL_SCANCODE_A:keys[0x7] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDL_SCANCODE_S:keys[0x8] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDL_SCANCODE_D:keys[0x9] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDL_SCANCODE_F:keys[0xE] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDL_SCANCODE_Z:keys[0xA] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDL_SCANCODE_X:keys[0x0] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDL_SCANCODE_C:keys[0xB] = ev.type == SDL_KEYDOWN ? true: false;break;
        case SDL_SCANCODE_V:keys[0xF] = ev.type == SDL_KEYDOWN ? true: false;break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}
void play_sound(SDL_AudioDeviceID device,std::array<int16_t,SAMPLES>& buffer,std::array<uint8_t,SOUND_BUFFER_SIZE>& sound_buffer,double pitch){
    double phase = 0;
    int index;
    for(int i = 0;i < SAMPLES;i++){
        while (phase >= 1){
            phase -= 1;
        }
        index = static_cast<int>(phase * SOUND_BUFFER_SIZE);
        buffer[i] = sound_buffer[index] ? 3000 : -3000;
        phase += pitch / SPEAKER_FREQ;
    }
    SDL_QueueAudio(device,buffer.data(),buffer.size() * sizeof(int16_t));
}
int main(int argc, char* argv[]){
    if (argc != 2){
        std::cout << "give rom path\n";
        return 1;
    }
    Chip8 chip;
    GraphicsLib graphicslib;
    chip.load_rom(argv[1]);
    SDL_Event ev;
    bool ring = false;
    uint64_t start, end, ms_per_frame = 1000 / FPS;
    SDL_AudioSpec spec;
    spec.freq = SPEAKER_FREQ;
    spec.format = AUDIO_S16SYS;
    spec.channels = 1;
    spec.samples = SAMPLES;
    spec.callback = nullptr;
    SDL_AudioDeviceID device = SDL_OpenAudioDevice(nullptr,0,&spec,nullptr,0);
    SDL_PauseAudioDevice(device,0);
    std::array<int16_t,SAMPLES> buffer;
    int instructions_per_frame = INSTRUCTION_RATE / FPS;
    while(true){
        start = SDL_GetTicks64();
        if (SDL_PollEvent(&ev) != 0){
            ImGui_ImplSDL2_ProcessEvent(&ev);
            process_event(chip.ret_keys(),ev);
        }
        graphicslib.create_frame();
        graphicslib.create_menu_ui();

        for (int i = 0; i < instructions_per_frame; i++){
            chip.decode_execute();
        }
        if (chip.has_resChanged()){
            chip.set_has_resChanged(false);
            graphicslib.setRes(chip.getQuirks().high_res);
        }
        graphicslib.printscreen(chip.ret_display(),chip.getQuirks().high_res);
        end = SDL_GetTicks64();
        if (end - start < ms_per_frame){
            SDL_Delay(ms_per_frame - (end - start));
        }
        ring = chip.update_timers();
        if (ring){
            play_sound(device,buffer,chip.get_sound_buffer(),chip.get_pitch());
        }
    }
}
