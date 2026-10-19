#pragma once

#include <SDL.h>
#include <cstdint>
#include <array>

#include "renderer.hpp"
#include "sound.hpp"
#include "input.hpp"

class Interface {
    private:
        Renderer* renderer;
        Sound* sound;
        Input* input;
        SDL_Window* sdlWindow = nullptr;
        SDL_Renderer* sdlRenderer = nullptr;
        SDL_AudioDeviceID audioDevice = 0;

        double phase = 0.0;
        array<int16_t, 2048> samples{};

        void decodeKey(SDL_Keycode key, bool state);

    public:
        Interface(Renderer* renderer, Sound* sound, Input* input);
        ~Interface(); // Destrutor
        void renderWindow();
        void updateSound();
        bool processEvents(); // Retorna false para fechar a tela
};