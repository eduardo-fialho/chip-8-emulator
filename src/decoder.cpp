#include "./include/decoder.hpp"
#include "./include/cpu.hpp"

#include <stdexcept>
#include <random>

uint16_t Decoder::extractData(uint16_t instruction, uint8_t num_nibbles) {
    if(num_nibbles > 3 || num_nibbles == 0) 
        throw invalid_argument("Number of nibbles must be 1 to 3");
    
    uint16_t data = 0;
    uint16_t mask = 0x000F;
    for(int i = 3 - num_nibbles; i < 3; i++) {
        data = data | (instruction & mask);
        mask = mask << 4;
    }
        
    return data; 
}

uint8_t Decoder::extractXIndex(uint16_t instruction) {
    return (instruction & 0xF00) >> 8;
}

uint8_t Decoder::extractYIndex(uint16_t instruction) {
    return (instruction & 0xF0) >> 4;
}

uint8_t Decoder::extractFirstDigit(uint16_t instruction) {
    return (instruction & 0xF000) >> 12;
}

bool Decoder::execute(uint16_t instruction) {
    uint16_t identificator = instruction; 
    
    if(instruction != 0x00E0 && instruction != 0x00EE)  
        identificator = extractFirstDigit(instruction);
    
    switch(identificator) {

    case 0xF:
        identificator = extractData(instruction, 2);
        switch(identificator) {
            
            case 0x1E: case 0x29: case 0x33: 
            case 0x55: case 0x65:
                executeTransfer(instruction);        
                break;
            
            case 0x0A:
                return executeInput(instruction);

            case 0x07: case 0x15: case 0x18:
                executeTemporizer(instruction);
                break;

            default:
                throw invalid_argument("Invalid 0xF instruction");
        }
        break;

    case 0x8:
        identificator = extractData(instruction, 1);
        switch(identificator) {
            case 0x0:
                executeTransfer(instruction);
                break;
            
            case 0x1: case 0x2: case 0x3: case 0x4:
            case 0x5: case 0x6: case 0x7: case 0xE:
                executeLogic(instruction);
                break;

            default:
                throw invalid_argument("Invalid 0x8 instruction");
        }
        break;

    case 0x0: case 0x1: case 0x2: case 0xB:
    case 0x00EE:
        executeFlow(instruction);
        break;
    
    case 0x3: case 0x4: case 0x5: case 0x9:
        executeCompare(instruction);
        break;
    
    case 0x6: case 0xA: 
        executeTransfer(instruction);
        break;
    
    case 0x7: case 0xC:
        executeLogic(instruction);
        break;
    
    case 0xD: case 0x00E0:
        executeGraphic(instruction);
        break;
    
    case 0xE:
        return executeInput(instruction);

    default:
        throw invalid_argument("Invalid instruction");
    }

    return true;
}

void Decoder::executeFlow(uint16_t instruction) {
    
    uint16_t identificator = instruction;

    if(instruction != 0x00EE) identificator = extractFirstDigit(instruction);
    
    uint16_t data_3 = extractData(instruction, 3);

    switch(identificator) {
        case 0x1:
            cpu->setPC(data_3);
            break;

        case 0x2:
            cpu->addAddressToStack(cpu->getPC());
            cpu->setPC(data_3);
            break;
        
        case 0x00EE:
            cpu->setPC(cpu->takeAddressFromStack());
            break;
        
        case 0xB:
            cpu->setPC(data_3 + cpu->getV(0));
            break;

        case 0x0:
            break;

        default:
            throw invalid_argument("Invalid Flow instruction");
    }
}

void Decoder::executeCompare(uint16_t instruction) {
    
    uint16_t identificator = extractFirstDigit(instruction);
    
    uint8_t data_1 = extractData(instruction, 1);
    uint8_t data_2 = extractData(instruction, 2);

    if((identificator == 0x5 || identificator == 0x9) && data_1 != 0)
        throw invalid_argument("Invalid Compare instruction");

    switch(identificator) {
        case 0x3:
            if(data_2 == cpu->getV(extractXIndex(instruction))) 
                cpu->setPC(cpu->getPC() + 2);
            break;

        case 0x4:
            if(data_2 != cpu->getV(extractXIndex(instruction))) 
                cpu->setPC(cpu->getPC() + 2);
            break;
        
        case 0x5:
            if(cpu->getV(extractXIndex(instruction)) == cpu->getV(extractYIndex(instruction))) 
                cpu->setPC(cpu->getPC() + 2);
            break;

        case 0x9:
            if(cpu->getV(extractXIndex(instruction)) != cpu->getV(extractYIndex(instruction))) 
                cpu->setPC(cpu->getPC() + 2);
            break;

        default:
            throw invalid_argument("Invalid Compare instruction");
    }
}

void Decoder::executeTransfer(uint16_t instruction) {
    
    uint16_t identificator = extractFirstDigit(instruction);
    
    uint8_t data_1 = extractData(instruction, 1);
    uint8_t data_2 = extractData(instruction, 2);
    uint16_t data_3 = extractData(instruction, 3);

    if(identificator == 0x8 && data_1 != 0) 
        throw invalid_argument("Invalid 0x8 Transfer instruction");

    switch(identificator) {
        case 0x6:
            cpu->setV(extractXIndex(instruction), data_2);
            break;
        
        case 0x8:
            cpu->setV(extractXIndex(instruction), cpu->getV(extractYIndex(instruction)));
            break;

        case 0xA:
            cpu->setI(data_3);
            break;

        case 0xF:
            switch(data_2) {
                case 0x1E:
                    cpu->setI(cpu->getV(extractXIndex(instruction)) + cpu->getI());
                    break;

                case 0x29:
                    if(cpu->getV(extractXIndex(instruction)) > 15) throw invalid_argument("Invalid Transfer instruction");
                    cpu->setI(0x50 + cpu->getV(extractXIndex(instruction)) * 5);
                    break;
                
                case 0x33:
                    memory->write(cpu->getI(), static_cast<uint8_t>(cpu->getV(extractXIndex(instruction)) / 100));
                    memory->write(cpu->getI() + 1, static_cast<uint8_t>((cpu->getV(extractXIndex(instruction)) % 100) / 10));
                    memory->write(cpu->getI() + 2, static_cast<uint8_t>((cpu->getV(extractXIndex(instruction)) % 10)));
                    break;

                case 0x55:
                    for(uint8_t i = 0; i <= extractXIndex(instruction); i++)
                        memory->write(cpu->getI() + i, cpu->getV(i));
                    break;

                case 0x65:
                    for(uint8_t i = 0; i <= extractXIndex(instruction); i++)
                        cpu->setV(i, memory->read(cpu->getI() + i));
                    break;

                default: 
                    throw invalid_argument("Invalid 0xF Transfer instruction");
            }
            break;

        default:
            throw invalid_argument("Invalid Transfer instruction");
    }
}

void Decoder::executeLogic(uint16_t instruction) {

    uint8_t identificator = extractFirstDigit(instruction);
    uint8_t x = extractXIndex(instruction);
    uint8_t nn = extractData(instruction, 2);

    switch (identificator) {
        case 0x7: {
            uint16_t sum = cpu->getV(x) + nn;
            cpu->setV(x, static_cast<uint8_t>(sum));
            break;
        }

        case 0xC: {
            static mt19937 generator(random_device{}());
            static uniform_int_distribution<unsigned int> distribution(0, 255);

            uint8_t randomByte = static_cast<uint8_t>(distribution(generator));
            cpu->setV(x, randomByte & nn);
            break;
        }

        case 0x8: {
            uint8_t subOpcode = extractData(instruction, 1);
            uint8_t y = extractYIndex(instruction);

            uint8_t vx = cpu->getV(x);
            uint8_t vy = cpu->getV(y);

            switch (subOpcode) {
                case 0x1:
                    cpu->setV(x, vx | vy);
                    break;

                case 0x2:
                    cpu->setV(x, vx & vy);
                    break;

                case 0x3:
                    cpu->setV(x, vx ^ vy);
                    break;

                case 0x4: {
                    uint16_t sum = static_cast<uint16_t>(vx) + vy;
                    cpu->setV(x, static_cast<uint8_t>(sum));
                    cpu->setV(0xF, sum > 0xFF ? 1 : 0);
                    break;
                }

                case 0x5: {
                    bool noBorrow = vx >= vy;
                    uint8_t result = static_cast<uint8_t>(vx - vy);

                    cpu->setV(x, result);
                    cpu->setV(0xF, noBorrow ? 1 : 0);
                    break;
                }

                case 0x6: {
                    uint8_t leastSignificantBit = vx & 0x01;

                    cpu->setV(x, vx >> 1);
                    cpu->setV(0xF, leastSignificantBit);
                    break;
                }

                case 0x7: {
                    bool noBorrow = vy >= vx;
                    uint8_t result = static_cast<uint8_t>(vy - vx);

                    cpu->setV(x, result);
                    cpu->setV(0xF, noBorrow ? 1 : 0);
                    break;
                }

                case 0xE: {
                    uint8_t mostSignificantBit = (vx & 0x80) != 0;
                    uint8_t result = static_cast<uint8_t>(
                        static_cast<unsigned int>(vx) << 1
                    );

                    cpu->setV(x, result);
                    cpu->setV(0xF, mostSignificantBit);
                    break;
                }

                default:
                    throw invalid_argument("Invalid 0x8 Logic instruction");
            }

            break;
        }

        default:
            throw invalid_argument("Invalid Logic instruction");
    }
}

void Decoder::executeGraphic(uint16_t instruction) {
    
    uint16_t identificator = instruction;

    if(instruction != 0x00E0) identificator = extractFirstDigit(instruction);
    
    uint8_t data_1 = extractData(instruction, 1);

    switch(identificator) {
        case 0x00E0:
            renderer->clear();
            break;

        case 0xD: {
            vector<uint8_t> bytes;
            for(int i = 0; i < data_1; i++) {
                bytes.push_back(
                    memory->read(cpu->getI() + i)
                );
            }

            if(renderer->draw(
                cpu->getV(extractXIndex(instruction)), 
                cpu->getV(extractYIndex(instruction)),
                bytes
            )) cpu->setV(15, 1);
            else cpu->setV(15, 0);

            break;
        }

        default:
            throw invalid_argument("Invalid Graphic instruction");
    }
}

bool Decoder::executeInput(uint16_t instruction) {
    
    uint8_t data_2 = extractData(instruction, 2);
    uint16_t identificator = data_2;

    switch(identificator) {
        case 0x9E:
            if(input->getKeyState(cpu->getV(extractXIndex(instruction)) & 0xF)) 
                cpu->setPC(cpu->getPC() + 2);
            break;

        case 0xA1:
            if(!input->getKeyState(cpu->getV(extractXIndex(instruction)) & 0xF)) 
                cpu->setPC(cpu->getPC() + 2);
            break;

        case 0x0A: {
            optional<uint8_t> pending_key = input->consumePendingEvent(); 
            if(pending_key.has_value()) {
                cpu->setV(extractXIndex(instruction), pending_key.value());
                return true;
            }
            return false;
        }

        default:
            throw invalid_argument("Invalid Input instruction");
    }
    return true;
}

void Decoder::executeTemporizer(uint16_t instruction) {
    
    uint16_t identificator = extractData(instruction, 2);

    switch(identificator) {
        case 0x07:
            cpu->setV(extractXIndex(instruction), cpu->getDT());
            break;

        case 0x15:
            cpu->setDT(cpu->getV(extractXIndex(instruction)));
            break;

        case 0x18: 
            cpu->setST(cpu->getV(extractXIndex(instruction)));
            break;

        default:
            throw invalid_argument("Invalid Temporizer instruction");
    }
}