#include "./include/memory.hpp"

#include <stdexcept>

Memory::Memory()
{
    reset();
}

void Memory::reset()
{
    ram.fill(0);
    static constexpr array<uint8_t, 80> font = {
        // 0
        0xF0, 0x90, 0x90, 0x90, 0xF0,
        // 1
        0x20, 0x60, 0x20, 0x20, 0x70,
        // 2
        0xF0, 0x10, 0xF0, 0x80, 0xF0,
        // 3
        0xF0, 0x10, 0xF0, 0x10, 0xF0,
        // 4
        0x90, 0x90, 0xF0, 0x10, 0x10,
        // 5
        0xF0, 0x80, 0xF0, 0x10, 0xF0,
        // 6
        0xF0, 0x80, 0xF0, 0x90, 0xF0,
        // 7
        0xF0, 0x10, 0x20, 0x40, 0x40,
        // 8
        0xF0, 0x90, 0xF0, 0x90, 0xF0,
        // 9
        0xF0, 0x90, 0xF0, 0x10, 0xF0,
        // A
        0xF0, 0x90, 0xF0, 0x90, 0x90,
        // B
        0xE0, 0x90, 0xE0, 0x90, 0xE0,
        // C
        0xF0, 0x80, 0x80, 0x80, 0xF0,
        // D
        0xE0, 0x90, 0x90, 0x90, 0xE0,
        // E
        0xF0, 0x80, 0xF0, 0x80, 0xF0,
        // F
        0xF0, 0x80, 0xF0, 0x80, 0x80};

    for (uint16_t i = 0; i < font.size(); i++)
        ram[0x050 + i] = font[i];
}

bool Memory::isValidAddress(uint16_t address) const
{
    return address <= 0xFFF;
}

void Memory::write(uint16_t address, uint8_t value)
{
    if (isValidAddress(address))
    {
        ram[address] = value;
        return;
    }
    throw out_of_range("Invalid memory address");
}

uint8_t Memory::read(uint16_t address) const
{
    if (isValidAddress(address))
        return ram[address];
    throw out_of_range("Invalid memory address");
}

void Memory::load(uint16_t initialAddress, const vector<uint8_t> &programBytes)
{
    if (!isValidAddress(initialAddress))
        throw out_of_range("Invalid initial address");

    if (programBytes.size() > ram.size() - initialAddress)
        throw length_error("Program larger than memory");

    reset();

    for (uint16_t i = initialAddress; i < programBytes.size() + initialAddress; i++)
        ram[i] = programBytes[i - initialAddress];
}