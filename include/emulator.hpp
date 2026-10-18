#pragma once

#include <memory>
#include <string>

#include "cpu.hpp"
#include "temporizer.hpp"
#include "memory.hpp"
#include "interface.hpp"
#include "decoder.hpp"
#include "sound.hpp"
#include "input.hpp"
#include "renderer.hpp"

using namespace std;

class Emulator {
    private:
        unique_ptr<Memory> memory;
        unique_ptr<Temporizer> temporizer;
        unique_ptr<Renderer> renderer;
        unique_ptr<Input> input;
        unique_ptr<Sound> sound;
        unique_ptr<CPU> cpu;
        unique_ptr<Interface> interface;
        unique_ptr<Decoder> decoder;

    public:
        Emulator();
        void start(const string &programPath);

};