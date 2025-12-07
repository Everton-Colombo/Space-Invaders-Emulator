#include "cpu8080.hpp"
#include <iostream>
#include <cstdio>

uint8_t* CPU_8080::_getAddr(SrcDestId8080 id) {
    switch (id) {
        case SrcDestId8080::A: return &this->registers.a;
        case SrcDestId8080::B: return &this->registers.b;
        case SrcDestId8080::C: return &this->registers.c;
        case SrcDestId8080::D: return &this->registers.d;
        case SrcDestId8080::E: return &this->registers.e;
        case SrcDestId8080::H: return &this->registers.h;
        case SrcDestId8080::L: return &this->registers.l;

        case SrcDestId8080::M: return &this->memory[(static_cast<uint16_t>(registers.h) << 8) | registers.l];
    }
} 

void CPU_8080::opLXI(RegisterPairId8080 rp, uint8_t dl, uint8_t dh) {
    registers.setPair(rp, dl, dh);
    registers.pc += 2;
}

void CPU_8080::opMOV(SrcDestId8080 dest, SrcDestId8080 src) {
    *_getAddr(dest) = *_getAddr(src);
}

void CPU_8080::opMVI(SrcDestId8080 dest, uint8_t data) {
    *_getAddr(dest) = data;
    registers.pc++;
}

bool CPU_8080::tick() {
    uint8_t* opcode = &this->memory[this->registers.pc];
    uint8_t nibble0 = (*opcode) & 0b11110000;
    uint8_t nibble1 = (*opcode) & 0b00001111;

    if (nibble0 <= 3) {
        if (nibble1 == 6 || nibble1 == 0xE) {
            // MVI 0|0||D|D|D|1|1|0 + d8
            opMVI(static_cast<SrcDestId8080>(*opcode & 0b00111000), opcode[1]);
        }

        if (nibble1 = 1) {
            // LXI 0|0|R|P|0|0|0|1 + d16
            opLXI(static_cast<RegisterPairId8080>(*opcode & 0b00110000), opcode[1], opcode[2]);
        }
    }

    if (nibble0 >= 4 && nibble0 <= 7) {
        if (nibble1 == 6) { /* HLT */ }

        // MOV 0|1|D|D|D|S|S|S
        opMOV(static_cast<SrcDestId8080>(*opcode & 0b00111000), static_cast<SrcDestId8080>(*opcode & 0b00000111));
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

    registers.pc++;
}