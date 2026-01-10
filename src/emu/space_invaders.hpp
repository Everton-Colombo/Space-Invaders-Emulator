#pragma once
#include "cpu8080.hpp"

class SpaceInvadersMachine {
private:
    CPU_8080 cpu;

    // Shift register
    uint8_t shiftOffset;
    uint8_t shift0;
    uint8_t shift1;

    uint8_t cpuIoInHandler(uint8_t port);
    void cpuIoOutHandler(uint8_t port, uint8_t value);


};