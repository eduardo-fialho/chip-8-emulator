#pragma once

#include <cstdint>

class CPU;
#include "input.hpp"
#include "memory.hpp"
#include "renderer.hpp"

using namespace std;

class Decoder {
    private:
        CPU* cpu;
        Memory* memory;
        Renderer* renderer;
        Input* input;

        void executeFlow(uint16_t instruction);
        void executeTransfer(uint16_t instruction);
        void executeLogic(uint16_t instruction);
        void executeCompare(uint16_t instruction);
        void executeGraphic(uint16_t instruction);
        bool executeInput(uint16_t instruction);
        void executeTemporizer(uint16_t instruction);

        uint16_t extractData(uint16_t instruction, uint8_t num_nibbles);
        uint8_t extractXIndex(uint16_t instruction);
        uint8_t extractYIndex(uint16_t instruction);
        uint8_t extractFirstDigit(uint16_t instruction);

    public:

        Decoder(CPU* cpu, Memory* memory, Renderer* renderer, Input* input);

        bool execute(uint16_t instruction); // False significa que a instrução está aguardando uma tecla
};