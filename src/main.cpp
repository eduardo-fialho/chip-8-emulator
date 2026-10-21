#include "./include/emulator.hpp"

#include <stdexcept>

int main(int argc, char* argv[]) {

    if(argc != 2)
        throw invalid_argument("Invalid argument");

    Emulator emulator{};
    emulator.start(argv[1]);

    return 0;
}