#pragma once
#include <stdint.h>

class SpaceInvadersShiftRegister {
private:
    uint8_t shiftOffset;
    uint8_t shift0;
    uint8_t shift1;

public:
    uint8_t getResult();
    void fill(uint8_t value);
    void setOffset(uint8_t value);
};