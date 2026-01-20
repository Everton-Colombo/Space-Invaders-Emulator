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

class SpaceInvadersBus : public BUS_8080 {
private:
    uint8_t* rom;
    uint8_t* workRam;
    uint8_t* videoRam;

public:
    SpaceInvadersBus(uint8_t* rom, uint8_t* workRam, uint8_t* videoRam) : rom(rom), workRam(workRam), videoRam(videoRam) {};

    uint8_t& operator[](size_t index) override {
        if (index <= 0x1FFF) {
            return rom[index];
        } else if (index <= 0x23FF) {
            return workRam[index - 0x2000];
        } else {
            return videoRam[index - 0x2000 - 0x2400];
        }
    };
};

class SpaceInvadersMachine {
private:
    SpaceInvadersBus bus;
    CPU_8080 cpu;
    uint8_t cpuIoInHandler(uint8_t port);
    void cpuIoOutHandler(uint8_t port, uint8_t value);

    uint8_t workRam[1024];  // 1 KB
    uint8_t videoRam[7168]; // 7 KB

    // Peripherals:
    SpaceInvadersShiftRegister shiftRegister;
    SpaceInvadersControllerInput controllerInput;

public:
    SpaceInvadersMachine(uint8_t* rom) 
        : bus(rom, workRam, videoRam),
          cpu(&bus,
              [this](uint8_t port) { return cpuIoInHandler(port); },
              [this](uint8_t port, uint8_t value) { cpuIoOutHandler(port, value); }) {}
};