#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "utils.hpp"
#include "cpu8080.hpp"
#include "space_invaders.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: cpu8080_debugger <rom_path>" << std::endl;
        return 1;
    }

    std::vector<uint8_t> rom = loadROM(argv[1]);
    SpaceInvadersBus bus = SpaceInvadersBus(rom.data(), new uint8_t[1024], new uint8_t[7096]);
    CPU_8080 cpu8080(&bus, true);

    uint64_t cycleCount = 0;
    uint32_t step = 1;
    std::string input;
    do {
        cpu8080._printState();
        std::cout << std::endl;
        if (cycleCount % step == 0) {
            step = 1;
            std::getline(std::cin, input);
            if (!input.empty()) {
                std::istringstream stream(input);
                stream >> step;
            }
        }
        cycleCount++;
    } while (cpu8080.executeNext());

    return 0;
}