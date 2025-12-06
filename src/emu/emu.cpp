#include "emu.hpp"
#include <iostream>
#include <cstdio>

bool CPU_8080::tick() {
    uint8_t* opcode = &this->memory[this->registers.pc];

    switch (*opcode) {
        default:
            printf("ERROR: Unimplemented instruction (%02x)\n", *opcode);
            return false;
    }
}