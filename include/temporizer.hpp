#pragma once

#include <chrono>

#define REGISTER_CLOCK 60 // 60Hz

using namespace std;

class Temporizer {
    private:

        unsigned int IPS = 540; // Instructions Per Second (Default)

        chrono::steady_clock::time_point startTime;
        chrono::steady_clock::time_point lastTime;

        chrono::steady_clock::duration registerAccumulator;
        chrono::steady_clock::duration instructionAccumulator;

    public:
        void start();
        void start(unsigned int IPS);
        void reset();
        void updateTime();
        unsigned int consumeRegisterAccumulator();
        unsigned int consumeInstructionAccumulator();
};