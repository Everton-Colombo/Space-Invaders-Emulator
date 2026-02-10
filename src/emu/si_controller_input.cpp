#include "si_controller_input.hpp"

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