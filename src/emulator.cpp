#include "./include/emulator.hpp"

#include <fstream>
#include <stdexcept>
#include <vector>
#include <iterator>

Emulator::Emulator() {
    memory = make_unique<Memory>();
    temporizer = make_unique<Temporizer>();
    renderer = make_unique<Renderer>();
    input = make_unique<Input>();
    sound = make_unique<Sound>();
    cpu = make_unique<CPU>(memory.get());
    decoder = make_unique<Decoder>(cpu.get(), memory.get(), renderer.get(), input.get());
    cpu->connectDecoder(decoder.get());
    interface = make_unique<Interface>(renderer.get(), sound.get(), input.get());
}

void Emulator::start(const string &programPath) {
    
    auto file = ifstream(programPath, ios_base::binary);

    if(!file.is_open())
        throw invalid_argument("Cannot open the program");

    vector<uint8_t> data(
        (istreambuf_iterator<char>(file)),
        istreambuf_iterator<char>()
    );

    if(data.size() > 4 * 1024 - 512) 
        throw invalid_argument("Program larger than CHIP-8 memory");

    memory->load(0x200, data);
    cpu->reset();
    temporizer->start();

    while(interface->processEvents()) {
        temporizer->updateTime();

        unsigned int instructions = temporizer->consumeInstructionAccumulator();
        for(unsigned int i = 0; i < instructions; i++)
            cpu->step();

        unsigned int registerUpdates = temporizer->consumeRegisterAccumulator();
        for(unsigned int i = 0; i < registerUpdates; i++)
            cpu->updateTemporizerRegisters();

        sound->setActive(static_cast<bool>(cpu->getST()));
        interface->updateSound();
        interface->renderWindow();
    }
}