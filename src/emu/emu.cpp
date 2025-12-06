#include "emu.hpp"
#include <iostream>
#include <cstdio>

uint8_t* CPU_8080::_getByteAddr(RegisterId8080 id) {
    switch (id) {
        case RegisterId8080::A: return &this->registers.a;
        case RegisterId8080::B: return &this->registers.b;
        case RegisterId8080::C: return &this->registers.c;
        case RegisterId8080::D: return &this->registers.d;
        case RegisterId8080::E: return &this->registers.e;
        case RegisterId8080::H: return &this->registers.h;
        case RegisterId8080::L: return &this->registers.l;

        case RegisterId8080::M: return &this->memory[registers.h << 8 | registers.l];
    }
}

void CPU_8080::opMOV(RegisterId8080 dest, RegisterId8080 src) {
    *_getByteAddr(dest) = *_getByteAddr(src);
}

bool CPU_8080::tick() {
    uint8_t* opcode = &this->memory[this->registers.pc];
    uint8_t nibble0 = (*opcode) & 0b11110000;
    uint8_t nibble1 = (*opcode) & 0b00001111;

    if (nibble0 >= 4 && nibble0 <= 7) {
        if (nibble1 == 6) { /* HLT */ }

        // MOV 0|1|D|D|D|S|S|S
        opMOV(static_cast<RegisterId8080>(*opcode & 0b00111000), static_cast<RegisterId8080>(*opcode & 0b00000111));
        return true;
    }

    switch (*opcode) {
        case 0x00:
        case 0x10:
        case 0x20:
        case 0x30:
        case 0x08:
        case 0x18:
        case 0x28:
        case 0x38:
            break; // NOP

        default:
            printf("ERROR: Unimplemented instruction (%02x)\n", *opcode);
            return false;
    }

    this->registers.pc++;
}