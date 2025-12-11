#include "cpu8080.hpp"
#include <iostream>
#include <cstdio>

uint8_t* CPU_8080::_getAddr(SrcDestId8080 id) {
    switch (id) {
        case SrcDestId8080::A: return &registers.a;
        case SrcDestId8080::B: return &registers.b;
        case SrcDestId8080::C: return &registers.c;
        case SrcDestId8080::D: return &registers.d;
        case SrcDestId8080::E: return &registers.e;
        case SrcDestId8080::H: return &registers.h;
        case SrcDestId8080::L: return &registers.l;

        case SrcDestId8080::M: return &memory[(static_cast<uint16_t>(registers.h) << 8) | registers.l];
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

// Arithemetic Ops
bool determineParity(uint8_t n) {
    int count = 0;
    while (n > 0) {
        n &= (n - 1);
        count++;
    }
    return count % 2;
}

void CPU_8080::_setArithmeticConditionFlags(uint16_t operationResult) {
    conditionFlags.z = ((operationResult & 0xff) == 0);
    conditionFlags.s = ((operationResult & 0x80) != 0);
    conditionFlags.cy = (operationResult > 0xff);
    conditionFlags.p = determineParity(operationResult);
}

void CPU_8080::opADD(SrcDestId8080 src) {
    uint16_t result = static_cast<uint16_t>(registers.a) + static_cast<uint16_t>(*_getAddr(src));
    registers.a = result & 0xff;

    _setArithmeticConditionFlags(result);
}

void CPU_8080::opADI(uint8_t data) {
    uint16_t result = static_cast<uint16_t>(registers.a) + static_cast<uint16_t>(data);
    registers.a = result & 0xff;

    _setArithmeticConditionFlags(result);

    registers.pc += 1;
}

void CPU_8080::opADC(SrcDestId8080 src) {
    uint16_t result = static_cast<uint16_t>(registers.a) + static_cast<uint16_t>(*_getAddr(src)) + conditionFlags.cy;
    registers.a = result & 0xff;

    _setArithmeticConditionFlags(result);
}

void CPU_8080::opACI(uint8_t data) {
    uint16_t result = static_cast<uint16_t>(registers.a) + static_cast<uint16_t>(data) + conditionFlags.cy;
    registers.a = result & 0xff;

    _setArithmeticConditionFlags(result);

    registers.pc += 1;
}

void CPU_8080::opSUB(SrcDestId8080 src) {
    uint16_t result = static_cast<uint16_t>(registers.a) - static_cast<uint16_t>(*_getAddr(src));
    registers.a = result & 0xff;

    _setArithmeticConditionFlags(result);
}

void CPU_8080::opSUI(uint8_t data) {
    uint16_t result = static_cast<uint16_t>(registers.a) - static_cast<uint16_t>(data);
    registers.a = result & 0xff;

    _setArithmeticConditionFlags(result);

    registers.pc += 1;
}

void CPU_8080::opSBB(SrcDestId8080 src) {
    uint16_t result = static_cast<uint16_t>(registers.a) - static_cast<uint16_t>(*_getAddr(src)) - conditionFlags.cy;
    registers.a = result & 0xff;

    _setArithmeticConditionFlags(result);
}

void CPU_8080::opSBI(uint8_t data) {
    uint16_t result = static_cast<uint16_t>(registers.a) - static_cast<uint16_t>(data) - conditionFlags.cy;
    registers.a = result & 0xff;

    _setArithmeticConditionFlags(result);

    registers.pc += 1;
}

void CPU_8080::opANA(SrcDestId8080 src) {
    registers.a = registers.a & *_getAddr(src);
    _setArithmeticConditionFlags(registers.a);
    conditionFlags.cy = 0; // CY flag is cleared
}

void CPU_8080::opANI(uint8_t data) {
    registers.a = registers.a & data;
    _setArithmeticConditionFlags(registers.a);
    conditionFlags.cy = 0; // CY flag is cleared

    registers.pc += 1;
}

void CPU_8080::opORA(SrcDestId8080 src) {
    registers.a = registers.a | *_getAddr(src);
    _setArithmeticConditionFlags(registers.a);
    conditionFlags.cy = 0; // CY flag is cleared
    conditionFlags.ac = 0; // AC flag is cleared
}

void CPU_8080::opORI(uint8_t data) {
    registers.a = registers.a | data;
    _setArithmeticConditionFlags(registers.a);
    conditionFlags.cy = 0; // CY flag is cleared
    conditionFlags.ac = 0; // AC flag is cleared

    registers.pc += 1;
}

void CPU_8080::opXRA(SrcDestId8080 src) {
    registers.a = registers.a ^ *_getAddr(src);
    _setArithmeticConditionFlags(registers.a);
    conditionFlags.cy = 0; // CY flag is cleared
    conditionFlags.ac = 0; // AC flag is cleared
}

void CPU_8080::opXRI(uint8_t data) {
    registers.a = registers.a ^ data;
    _setArithmeticConditionFlags(registers.a);
    conditionFlags.cy = 0; // CY flag is cleared
    conditionFlags.ac = 0; // AC flag is cleared

    registers.pc += 1;
}

bool CPU_8080::tick() {
    uint8_t* opcode = &this->memory[this->registers.pc];
    uint8_t nibble0 = (*opcode) & 0b11110000;
    uint8_t nibble1 = (*opcode) & 0b00001111;

    if (nibble0 <= 3) {
        if (nibble1 = 1) {
            // LXI 0|0|R|P|0|0|0|1 + d16
            opLXI(static_cast<RegisterPairId8080>(*opcode & 0b00110000), opcode[1], opcode[2]);
        }
        
        if (nibble1 == 6 || nibble1 == 0xE) {
            // MVI 0|0||D|D|D|1|1|0 + d8
            opMVI(static_cast<SrcDestId8080>(*opcode & 0b00111000), opcode[1]);
        }
    }

    if (nibble0 >= 4 && nibble0 <= 7) {
        if (nibble1 == 6) { /* HLT */ }
        else
            opMOV(static_cast<SrcDestId8080>(*opcode & 0b00111000), static_cast<SrcDestId8080>(*opcode & 0b00000111)); // MOV 0|1|D|D|D|S|S|S
    }

    if (nibble0 == 8) {
        if (nibble1 <= 7)
            opADD(static_cast<SrcDestId8080>(*opcode & 0b00000111)); // ADD 1|0|0|0|0|S|S|S
        else
            opADC(static_cast<SrcDestId8080>(*opcode & 0b00000111)); // ADC 1|0|0|0|1|S|S|S 
    }

    if (nibble0 == 9) {
        if (nibble1 <= 7)
            opSUB(static_cast<SrcDestId8080>(*opcode & 0b00000111));
        else
            opSBB(static_cast<SrcDestId8080>(*opcode & 0b00000111));
    }

    if (nibble0 == 0xA) {
        if (nibble1 <= 7)
            opANA(static_cast<SrcDestId8080>(*opcode & 0b00000111));
        else
            opXRA(static_cast<SrcDestId8080>(*opcode & 0b00000111));
    }

    if (nibble0 == 0xB) {
        if (nibble1 <= 7)
            opORA(static_cast<SrcDestId8080>(*opcode & 0b00000111));
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
        
        case 0xC6:
            opADI(opcode[1]);
            break;
        
        case 0xCE:
            opACI(opcode[1]);
            break;
        
        case 0xD6:
            opSUI(opcode[1]);
            break;
        
        case 0xDE:
            opSBI(opcode[1]);
            break;
        
        case 0xE6:
            opANI(opcode[1]);
            break;
        
        case 0xEE:
            opXRI(opcode[1]);
            break;
        
        case 0xF6:
            opORI(opcode[1]);
            break;

        default:
            printf("ERROR: Unimplemented instruction (%02x)\n", *opcode);
            return false;
    }

    registers.pc++;
}