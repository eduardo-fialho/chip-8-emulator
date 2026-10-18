#pragma once

#include "renderer.hpp"
#include "sound.hpp"
#include "input.hpp"

class Interface {
    private:
        Renderer* renderer;
        Sound* sound;
        Input* input;

    public:
        Interface(Renderer* renderer, Sound* sound, Input* input);
        ~Interface(); // Destrutor
        void renderWindow();
        void updateSound();
        bool processEvents(); // Retorna false para fechar a tela
};