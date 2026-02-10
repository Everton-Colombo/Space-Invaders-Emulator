#pragma once
#include <array>
#include <stdint.h>

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