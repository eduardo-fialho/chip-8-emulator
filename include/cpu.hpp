#pragma once

#include <cstdint>
#include <array>
#include <stack>

class Decoder;
#include "memory.hpp"

#define MAX_STACK_SIZE 16

using namespace std;

class CPU {
    private:

        array<uint8_t, 16> V{};
        uint16_t I = 0;
        uint16_t PC = 0x200;
        uint8_t DT = 0;
        uint8_t ST = 0;

        stack<uint16_t> stack{};

        Decoder* decoder = nullptr;
        Memory* memory;

        uint16_t concatenateBytes(uint8_t high, uint8_t low);
        bool isValidVRegister(uint8_t index) const;

    public:

        CPU(Memory* memory);

        void connectDecoder(Decoder* decoder);

        uint16_t fetchInstruction(); // Não altera o PC
        void reset(); // Volta a CPU para o estado inicial
        void step(); // O PC é atualizado aqui
        void updateTemporizerRegisters();

        void setV(uint8_t index, uint8_t value);
        void setI(uint16_t value);
        void setPC(uint16_t value);
        void setDT(uint8_t value);
        void setST(uint8_t value);

        uint8_t getV(uint8_t index) const;
        uint16_t getI() const;
        uint16_t getPC() const;
        uint8_t getDT() const;
        uint8_t getST() const;

        void addAddressToStack(uint16_t address);
        uint16_t takeAddressFromStack();

};