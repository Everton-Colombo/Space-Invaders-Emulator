#pragma once
#include <array>
#include "cpu8080.hpp"
#include "si_shift_register.hpp"
#include "si_controller_input.hpp"
#include "si_display.hpp"
#include "si_audio_player.hpp"

#define SPACE_INVADERS_SCREEN_WIDTH  224
#define SPACE_INVADERS_SCREEN_HEIGHT 256

class SpaceInvadersBus : public IIndexable {
private:
    uint8_t* rom;
    uint8_t* workRam;
    uint8_t* videoRam;
    uint8_t oob = 0;

public:
    SpaceInvadersBus(uint8_t* rom, uint8_t* workRam, uint8_t* videoRam) : rom(rom), workRam(workRam), videoRam(videoRam) {};

    uint8_t& operator[](size_t index) override {
        if (index <= 0x1FFF) {
            return rom[index];
        } else if (index <= 0x23FF) {
            return workRam[index - 0x2000];
        } else if (index < 0x4000){
            return videoRam[index - 0x2400];
        } else {
            return oob;
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
    ISpaceInvadersDisplay* display;
    ISpaceInvadersAudioPlayer* audioPlayer;

public:
    SpaceInvadersMachine(uint8_t* rom, ISpaceInvadersDisplay* display, ISpaceInvadersAudioPlayer* audioPlayer)
        : bus(rom, workRam, videoRam),
          cpu(&bus,
              [this](uint8_t port) { return cpuIoInHandler(port); },
              [this](uint8_t port, uint8_t value) { cpuIoOutHandler(port, value); },
              true),
          display(display), audioPlayer(audioPlayer) {}

    void tick(); // 60 Hz

    SpaceInvadersControllerInput& getControllerInput() {
        return controllerInput;
    }

    const uint8_t* getVideoRam() const {
        return videoRam;
    }
};