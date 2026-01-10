#include "space_invaders.hpp"

uint8_t SpaceInvadersMachine::cpuIoInHandler(uint8_t port) {
    switch(port) {    
        case 3: // Shift Register result
            uint16_t v = (shift1<<8) | shift0;    
            return ((v >> (8 - shiftOffset)) & 0xff);    
    }    
}

void SpaceInvadersMachine::cpuIoOutHandler(uint8_t port, uint8_t value) {
    switch (port) {
        case 2: // shift register result offset (bits 0,1,2)
            shiftOffset = value & 0b111;
            break;
        case 4: // fill shift register
            shift0 = shift1;    
            shift1 = value;    
            break;
    }
}