#include "./include/cpu.hpp"
#include "./include/decoder.hpp"

#include <stdexcept>

CPU::CPU(Memory* memory) {
    if(memory == nullptr) throw invalid_argument("Memory is null");
    this->memory = memory;
}

void CPU::connectDecoder(Decoder* decoder) {
    if(decoder == nullptr) throw invalid_argument("Decoder is null");
    this->decoder = decoder;
}

void CPU::reset() {
    V.fill(0);
    I = 0;
    PC = 0x200;
    DT = 0;
    ST = 0;
    while (!stack.empty()) stack.pop();
} 
        
bool CPU::isValidVRegister(uint8_t index) const {
    if(index > 15) return false;
    return true;
}

uint16_t CPU::concatenateBytes(uint8_t high, uint8_t low) {
    return (high << 8) | low;
}

uint16_t CPU::fetchInstruction() {
    uint16_t instruction = concatenateBytes(memory->read(PC), memory->read(PC + 1));
    return instruction;
}

void CPU::step() {
    if(decoder == nullptr) throw invalid_argument("Decoder is null");
    uint16_t old_pc = PC;
    uint16_t instruction = fetchInstruction();
    PC += 2;
    if(!decoder->execute(instruction)) PC = old_pc;
}
        
void CPU::updateTemporizerRegisters() {
    if(DT != 0) DT--;
    if(ST != 0) ST--;
}

void CPU::setV(uint8_t index, uint8_t value) {
    if(!isValidVRegister(index)) throw out_of_range("Invalid V register index");
    V[index] = value;
}
        
void CPU::setI(uint16_t value) {
    I = value;
}
        
void CPU::setPC(uint16_t value) {
    PC = value;
}
        
void CPU::setDT(uint8_t value) {
    DT = value;
}
        
void CPU::setST(uint8_t value) {
    ST = value;
}

uint8_t CPU::getV(uint8_t index) const {
    if(!isValidVRegister(index)) throw out_of_range("Invalid V register index");
    return V[index];
}
        
uint16_t CPU::getI() const {
    return I;
}
        
uint16_t CPU::getPC() const {
    return PC;
}
        
uint8_t CPU::getDT() const {
    return DT;
}
        
uint8_t CPU::getST() const {
    return ST;
}

void CPU::addAddressToStack(uint16_t address) {
    if(stack.size() == 16) throw runtime_error("Stack overflow");
    stack.push(address);
}
        
uint16_t CPU::takeAddressFromStack() {
    if(stack.size() == 0) throw runtime_error("Stack underflow");
    uint16_t address = stack.top();
    stack.pop();
    return address;
}