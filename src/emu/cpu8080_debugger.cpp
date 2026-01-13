#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "cpu8080.hpp"

std::vector<uint8_t> loadROM(char* filePath) {
    std::ifstream fs(filePath, std::ios::binary | std::ios::ate);
    size_t size = fs.tellg();

    if (size <= 0)
        throw std::runtime_error("Error while opening rom.");
    
    fs.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> buffer(size);
    if (fs.read((char*)buffer.data(), size))
        return buffer;
    
    throw std::runtime_error("Error reading file content!");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: cpu8080_debugger <rom_path>" << std::endl;
        return 1;
    }

    std::vector<uint8_t> rom = loadROM(argv[1]);

    CPU_8080 cpu8080(rom.data(), true);

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
    } while (cpu8080.tick());

    return 0;
}