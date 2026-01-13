#include "space_invaders.hpp"

// Shift Register:
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


// Controller Input:
void SpaceInvadersControllerInput::setInputState(SpaceInvadersControllerInputId inputId, bool state) {
    inputStates[static_cast<int>(inputId)] = state;
}

const std::array<bool, 9> SpaceInvadersControllerInput::getInputStates() {
    return inputStates;
}

std::pair<uint8_t, uint8_t> SpaceInvadersControllerInput::getControllerInputPortsData() {
    uint8_t port1, port2;
    port1 = inputStates[COIN] | (inputStates[P2_START] << 1) | (inputStates[P1_START] << 2) | (inputStates[P1_SHOOT] << 4)
        | (inputStates[P1_LEFT] << 5) | (inputStates[P1_RIGHT] << 6);
    port2 = (inputStates[P2_SHOOT] << 4) | (inputStates[P2_LEFT] << 5) | (inputStates[P2_RIGHT] << 6);

    return {port1, port2};
}


// Machine:
uint8_t SpaceInvadersMachine::cpuIoInHandler(uint8_t port) {
    switch(port) {   
        case 1:
            return controllerInput.getControllerInputPortsData().first;
        case 2:
             return controllerInput.getControllerInputPortsData().second;
        case 3: // Shift Register result
            return shiftRegister.getResult();  
    }

    return 0;
}

void SpaceInvadersMachine::cpuIoOutHandler(uint8_t port, uint8_t value) {
    switch (port) {
        case 2: // shift register result offset (bits 0,1,2)
            shiftRegister.setOffset(value);
            break;
        case 4: // fill shift register
            shiftRegister.fill(value);
            break;
    }
}