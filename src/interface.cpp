#include "./include/interface.hpp"

#include <stdexcept>
#include <string>

Interface::Interface(Renderer* renderer, Sound* sound, Input* input) {
    if(renderer == nullptr) 
        throw invalid_argument("Renderer is null");
        
    if(sound == nullptr) 
        throw invalid_argument("Sound is null");

    if(input == nullptr) 
        throw invalid_argument("Input is null");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        throw runtime_error(SDL_GetError());

    SDL_Window *window = SDL_CreateWindow(
        "Chip-8 Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640,
        320,
        SDL_WINDOW_RESIZABLE
    );

    if(window == nullptr) {
        string error = SDL_GetError();
        SDL_Quit();
        throw runtime_error(error);
    }

    SDL_Renderer *sdl_renderer = SDL_CreateRenderer(window, -1, 0);

    if (sdl_renderer == nullptr) {
        string error = SDL_GetError();
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw runtime_error(error);
    }

    if (SDL_RenderSetLogicalSize(sdl_renderer, 64, 32) != 0) {
        string error = SDL_GetError();
        SDL_DestroyRenderer(sdl_renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw runtime_error(error);
    }

    SDL_AudioSpec audio_spec{};
    audio_spec.freq = 44100;
    audio_spec.format = AUDIO_S16SYS;
    audio_spec.channels = 1;
    audio_spec.samples = 2048;
    audio_spec.callback = nullptr;
    audio_spec.userdata = nullptr;

    audioDevice = SDL_OpenAudioDevice(nullptr, 0, &audio_spec, nullptr, 0);

    if(audioDevice == 0) {
        string error = SDL_GetError();
        SDL_DestroyRenderer(sdl_renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw runtime_error(error);    
    }

    SDL_PauseAudioDevice(audioDevice, 0);

    this->renderer = renderer;
    this->sound = sound;
    this->input = input;
    this->sdlWindow = window;
    this->sdlRenderer = sdl_renderer;
}

Interface::~Interface() {
    if(audioDevice != 0) SDL_CloseAudioDevice(audioDevice);
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
}

void Interface::renderWindow() {
    const auto& framebuffer = renderer->getFramebuffer();

    if(SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255) != 0) 
        throw runtime_error(SDL_GetError());

    if(SDL_RenderClear(sdlRenderer) != 0)
        throw runtime_error(SDL_GetError());

    if(SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 255, 255) != 0)
        throw runtime_error(SDL_GetError());

    for(int y = 0; y < 32; y++) {
        for(int x = 0; x < 64; x++) {
            if(framebuffer[y][x]) { 
                if(SDL_RenderDrawPoint(sdlRenderer, x, y) != 0)
                    throw runtime_error(SDL_GetError());
            }
        }
    }

    SDL_RenderPresent(sdlRenderer);
}

void Interface::decodeKey(SDL_Keycode key, bool state) {
    switch(key) {

    case SDLK_0: // 0
        input->updateKeyState(0, state);
        break;

    case SDLK_1: // 1
        input->updateKeyState(1, state);
        break;
    
    case SDLK_2: // 2
        input->updateKeyState(2, state);
        break;
    
    case SDLK_3: // 3
        input->updateKeyState(3, state);
        break;
    
    case SDLK_4: // 4
        input->updateKeyState(4, state);
        break;

    case SDLK_5: // 5
        input->updateKeyState(5, state);
        break;

    case SDLK_6: // 6
        input->updateKeyState(6, state);
        break;

    case SDLK_7: // 7
        input->updateKeyState(7, state);
        break;

    case SDLK_8: // 8
        input->updateKeyState(8, state);
        break;

    case SDLK_9: // 9
        input->updateKeyState(9, state);
        break;

    case SDLK_a: // A
        input->updateKeyState(10, state);
        break;

    case SDLK_b: // B
        input->updateKeyState(11, state);
        break;

    case SDLK_c: // C
        input->updateKeyState(12, state);
        break;

    case SDLK_d: // D
        input->updateKeyState(13, state);
        break;
    
    case SDLK_e: // E
        input->updateKeyState(14, state);
        break;

    case SDLK_f: // F
        input->updateKeyState(15, state);
        break;
    
    default:
        break;
    }
}


bool Interface::processEvents() {
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) return false;
        if(event.type == SDL_KEYDOWN) 
            decodeKey(event.key.keysym.sym, true);
        if(event.type == SDL_KEYUP) 
            decodeKey(event.key.keysym.sym, false);
    }

    return true;
}

void Interface::updateSound() {
    if(!sound->isActive()) {
        SDL_ClearQueuedAudio(audioDevice);
        return;
    }

    if (SDL_GetQueuedAudioSize(audioDevice) >= samples.size() * sizeof(int16_t))
        return;

    uint16_t frequency = sound->getFrequency(); 

    for (size_t i = 0; i < samples.size(); i++) {
        samples[i] = phase < 0.5 ? 12000 : -12000;
        phase += frequency / 44100.0;
        if(phase >= 1) phase -= 1.0;
    }

    if(SDL_QueueAudio(audioDevice, samples.data(), samples.size() * sizeof(int16_t)) != 0) {
        throw runtime_error(SDL_GetError());
    }
}