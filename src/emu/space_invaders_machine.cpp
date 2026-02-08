#include "space_invaders_machine.hpp"

#define CYCLES_PER_HALF_FRAME 16666

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
        case 3: // audio 1
            audioPlayer->interpretOutPort3(value);
            break;
        case 4: // fill shift register
            shiftRegister.fill(value);
            break;
        case 5: // audio 2
            audioPlayer->interpretOutPort5(value);
            break;
    }
}

void SpaceInvadersMachine::tick() {
    cpu.executeCycles(CYCLES_PER_HALF_FRAME);
    cpu.triggerInterrupt(1);

    cpu.executeCycles(CYCLES_PER_HALF_FRAME);
    cpu.triggerInterrupt(2);

    display->renderVram(videoRam);
}