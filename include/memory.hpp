#pragma once

#include <cstdint>
#include <array>
#include <vector>

using namespace std;

class Memory {
    private:
        array<uint8_t, 4 * 1024> ram{};

        bool isValidAddress(uint16_t address) const; // 0x000 até 0xFFF

    public:

        Memory(); // Prepara o estado inicial
        void reset(); // Volta ao estado inicial removendo o programa da memória

        void load(uint16_t initialAddress, const vector<uint8_t> &programBytes);

        uint8_t read(uint16_t address) const;
        void write(uint16_t address, uint8_t value);

};