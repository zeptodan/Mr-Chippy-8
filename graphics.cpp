#include"graphics.h"
GraphicsLib::GraphicsLib(){
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("Mr Chippy 8",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,CHIP_8_X * SCALE,CHIP_8_Y * SCALE + MENU_HEIGHT,SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,CHIP_8_X / 2,CHIP_8_Y / 2);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window,renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    ImGui::GetStyle().ScaleAllSizes(2.0f);
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 2.0f;
}
GraphicsLib::~GraphicsLib(){
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}
void GraphicsLib::printscreen(uint8_t* disp,bool high_res){
    int row_len = high_res ? CHIP_8_X : CHIP_8_X / 2;
    int col_len = high_res ? CHIP_8_Y : CHIP_8_Y / 2;
    int size = row_len * col_len;
    for (int i = 0,j = 0; i < size;i++,j++){
        if (j % CHIP_8_X >= row_len){
            int row_no = j / CHIP_8_X;
            j = (row_no + 1) * CHIP_8_X;
        }
        sdl_disp[i] = colors[disp[j]];
    }
    SDL_UpdateTexture(texture,NULL,sdl_disp.data(),row_len * sizeof(uint32_t));
    SDL_Rect rect{0,MENU_HEIGHT,CHIP_8_X * SCALE, CHIP_8_Y * SCALE};
    SDL_RenderCopy(renderer,texture,NULL,&rect);
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(),renderer);
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
}
void GraphicsLib::setRes(bool high_res){
    SDL_DestroyTexture(texture);
    if(high_res){
        texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,CHIP_8_X,CHIP_8_Y);
    }
    else{
        texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,CHIP_8_X / 2,CHIP_8_Y / 2);
    }
}
void GraphicsLib::create_frame(){
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}
void GraphicsLib::create_menu_ui(){
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open ROM")) {
                //open_rom();
            }
            if (ImGui::MenuItem("Reset")) {
                //reset_emulator();
            }
            if (ImGui::MenuItem("Quit")) {
                //running = false;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}