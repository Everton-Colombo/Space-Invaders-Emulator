#pragma once
#include <array>
#include "cpu8080.hpp"

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

enum SpaceInvadersControllerInputId {
    COIN,
    P1_START,
    P1_SHOOT,
    P1_LEFT,
    P1_RIGHT,
    P2_START,
    P2_SHOOT,
    P2_LEFT,
    P2_RIGHT
};

class SpaceInvadersControllerInput {
private:
    std::array<bool, 9> inputStates;

public:
    void setInputState(SpaceInvadersControllerInputId inputId, bool state);

    const std::array<bool, 9> getInputStates();
    std::pair<uint8_t, uint8_t> getControllerInputPortsData();
};

class SpaceInvadersMachine {
private:
    CPU_8080 cpu;
    uint8_t cpuIoInHandler(uint8_t port);
    void cpuIoOutHandler(uint8_t port, uint8_t value);

    // Peripherals:
    SpaceInvadersShiftRegister shiftRegister;
    SpaceInvadersControllerInput controllerInput;

};