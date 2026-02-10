#include "si_shift_register.hpp"

uint8_t SpaceInvadersShiftRegister::getResult() {
    uint16_t v = (shift1<<8) | shift0;    
    return ((v >> (8 - shiftOffset)) & 0xff);
}

void SpaceInvadersShiftRegister::fill(uint8_t value) {
    shift0 = shift1;    
    shift1 = value;  
}

void SpaceInvadersShiftRegister::setOffset(uint8_t value) {
    shiftOffset = value & 0b111;
}