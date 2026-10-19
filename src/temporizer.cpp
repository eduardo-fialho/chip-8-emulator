#include "./include/temporizer.hpp"

#include <stdexcept>
#include <ratio>

void Temporizer::start()
{
    reset();
    startTime = chrono::steady_clock::now();
}

void Temporizer::start(unsigned int IPS)
{
    if (IPS == 0)
        throw invalid_argument("IPS must be positive");
    start();
    this->IPS = IPS;
}

void Temporizer::reset()
{
    lastTime = chrono::steady_clock::now();
    registerAccumulator = chrono::steady_clock::duration::zero();
    instructionAccumulator = chrono::steady_clock::duration::zero();
}

void Temporizer::updateTime()
{
    chrono::steady_clock::time_point now = chrono::steady_clock::now();
    chrono::steady_clock::duration duration = now - lastTime;
    lastTime = now;
    registerAccumulator += duration;
    instructionAccumulator += duration;
}

unsigned int Temporizer::consumeRegisterAccumulator()
{

    chrono::duration<double> register_clock(1.0 / REGISTER_CLOCK);
    chrono::duration<double> accumulator_double = registerAccumulator;
    auto updates = accumulator_double / register_clock;
    unsigned int updates_int = static_cast<unsigned int>(updates);
    registerAccumulator -= chrono::duration_cast<chrono::steady_clock::duration>(register_clock * updates_int);

    return updates_int;
}

unsigned int Temporizer::consumeInstructionAccumulator()
{
    chrono::duration<double> register_clock(1.0 / IPS);
    chrono::duration<double> accumulator_double = instructionAccumulator;
    auto updates = accumulator_double / register_clock;
    unsigned int updates_int = static_cast<unsigned int>(updates);
    instructionAccumulator -= chrono::duration_cast<chrono::steady_clock::duration>(register_clock * updates_int);

    return updates_int;
}