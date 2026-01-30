#include "utils.hpp"
#include <fstream>

std::vector<uint8_t> loadROM(const char* filePath) {
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