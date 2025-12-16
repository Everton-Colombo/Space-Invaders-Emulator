#include "cpu8080.hpp"
#include <iostream>
#include <cstdio>
#include <utility>

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

// Data Transfer Group:

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

void CPU_8080::opLDA(uint8_t al, uint8_t ah) {
    registers.a = memory[(static_cast<uint16_t>(ah) << 8) | al];
    registers.pc += 2;
}

void CPU_8080::opSTA(uint8_t al, uint8_t ah) {
    memory[(static_cast<uint16_t>(ah) << 8) | al] = registers.a;
    registers.pc += 2;
}

void CPU_8080::opLHLD(uint8_t al, uint8_t ah) {
    uint16_t addr = (static_cast<uint16_t>(ah) << 8) | al;
    registers.l = memory[addr];
    registers.h = memory[addr + 1];

    registers.pc += 2;
}

void CPU_8080::opSHLD(uint8_t al, uint8_t ah) {
    uint16_t addr = (static_cast<uint16_t>(ah) << 8) | al;
    memory[addr] = registers.l;
    memory[addr + 1] = registers.h;

    registers.pc += 2;
}

void CPU_8080::opLDAX(RegisterPairId8080 rp) {
    registers.a = memory[registers.getPair(rp)];
}

void CPU_8080::opSTAX(RegisterPairId8080 rp) {
    memory[registers.getPair(rp)] = registers.a;
}

void CPU_8080::opXCHG() {
    std::swap(registers.h, registers.d);
    std::swap(registers.l, registers.e);
}

// Arithemetic Group:
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

void CPU_8080::opCMP(SrcDestId8080 src) {
    uint16_t result = static_cast<uint16_t>(registers.a) - *_getAddr(src);
    _setArithmeticConditionFlags(result);
}

void CPU_8080::opCPI(uint8_t data) {
    uint16_t result = static_cast<uint16_t>(registers.a) - data;
    _setArithmeticConditionFlags(result);

    registers.pc += 1;
}

void CPU_8080::opINX(RegisterPairId8080 rp) {
    registers.setPair(rp, registers.getPair(rp) + 1);
}

void CPU_8080::opDCX(RegisterPairId8080 rp) {
    registers.setPair(rp, registers.getPair(rp) - 1);
}

void CPU_8080::opINR(SrcDestId8080 dest) {
    uint8_t result = *_getAddr(dest) += 1;

    bool prevCy = conditionFlags.cy;
    _setArithmeticConditionFlags(result);
    conditionFlags.cy = prevCy; // CY isn't affected
}

void CPU_8080::opDCR(SrcDestId8080 dest) {
    uint8_t result = *_getAddr(dest) -= 1;

    bool prevCy = conditionFlags.cy;
    _setArithmeticConditionFlags(result);
    conditionFlags.cy = prevCy; // CY isn't affected
}

void CPU_8080::opRLC() {
    uint8_t a7Bit = registers.a >> 7;
    registers.a = (registers.a << 1) + a7Bit;
    conditionFlags.cy = a7Bit;  
}

void CPU_8080::opRRC() {
    uint8_t a0Bit = registers.a & 0b1;
    registers.a = (registers.a >> 1) + (a0Bit << 7);
    conditionFlags.cy = a0Bit;  
}

void CPU_8080::opRAL() {
    uint8_t a7Bit = registers.a >> 7;
    registers.a = (registers.a << 1) + conditionFlags.cy;
    conditionFlags.cy = a7Bit; 
}

void CPU_8080::opRAR() {
    uint8_t a0Bit = registers.a & 0b1;
    registers.a = (registers.a >> 1) + (conditionFlags.cy << 7);
    conditionFlags.cy = a0Bit; 
}

void CPU_8080::opCMA() {
    registers.a = ~registers.a;
}

void CPU_8080::opCMC() {
    conditionFlags.cy = ~conditionFlags.cy;
}

void CPU_8080::opSTC() {
    conditionFlags.cy = 1;
}

void CPU_8080::opDAD(RegisterPairId8080 rp) {
    uint32_t result = static_cast<uint32_t>(registers.getPair(HL)) + static_cast<uint32_t>(registers.getPair(rp));
    conditionFlags.cy = result > 0xffff;
    registers.setPair(HL, static_cast<uint16_t>(result));
}

void CPU_8080::opDAA() {
    if ((registers.a & 0x0f) > 9 || conditionFlags.ac) {
        registers.a += 6;
        conditionFlags.ac = 1;
    } else { conditionFlags.ac = 0; }

    if ((registers.a >> 4) > 9 || conditionFlags.cy) {
        registers.a += 6 << 4;
        conditionFlags.cy = 1;
    }

    conditionFlags.z = registers.a == 0;
    conditionFlags.s = ((registers.a & 0x80) != 0);
    conditionFlags.p = determineParity(registers.a);
}

// Branch Group:
bool CPU_8080::_evalCond(ConditionId8080 ccc) {
    switch (ccc) {
        case NZ: return !conditionFlags.z;
        case  Z: return  conditionFlags.z;
        case NC: return !conditionFlags.cy;
        case  C: return  conditionFlags.cy;
        case PO: return !conditionFlags.p;
        case PE: return  conditionFlags.p;
        case  P: return !conditionFlags.s;
        case  M: return  conditionFlags.s;  
    }
}

void CPU_8080::opJMP(uint8_t al, uint8_t ah) {
    registers.setPair(PC, al, ah);
}

void CPU_8080::opJcondition(ConditionId8080 ccc, uint8_t al, uint8_t ah) {
    if (_evalCond(ccc))
        registers.setPair(PC, al, ah);
}

void CPU_8080::opCALL(uint8_t al, uint8_t ah) {
    uint8_t pch = (registers.pc & 0xff00) >> 8;
    uint8_t pcl =  registers.pc & 0x00ff;

    memory[registers.sp - 1] = pch;
    memory[registers.sp - 2] = pcl;
    registers.sp -= 2;
    registers.setPair(PC, al, ah);
}

void CPU_8080::opCcondition(ConditionId8080 ccc, uint8_t al, uint8_t ah) {
    if (_evalCond(ccc))
        opCALL(al, ah);
}

bool CPU_8080::tick() {
    uint8_t* opcode = &this->memory[this->registers.pc];
    uint8_t nibble0 = (*opcode) & 0b11110000;
    uint8_t nibble1 = (*opcode) & 0b00001111;

    if (nibble0 <= 3) {
        if (nibble1 == 1) 
            opLXI(_getRp(*opcode), opcode[1], opcode[2]);

        if (nibble1 == 3)
            opINX(_getRp(*opcode));
        if (nibble1 == 0xB)
            opDCX(_getRp(*opcode));

        if (nibble1 == 4 || nibble1 == 0xC)
            opINR(_getDest(*opcode));
        
        if (nibble1 == 5 || nibble1 == 0xD)
            opDCR(_getDest(*opcode));
        
        if (nibble1 == 6 || nibble1 == 0xE)
            opMVI(_getDest(*opcode), opcode[1]);
        
        if (nibble1 == 9)
            opDAD(_getRp(*opcode));
        
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
            
            case 0x07: opRLC(); break;
            case 0x17: opRAL(); break;
            case 0x0F: opRRC(); break;
            case 0x1F: opRAR(); break;

            case 0x27: opDAA(); break;
            case 0x37: opSTC(); break;
            case 0x2F: opCMA(); break;
            case 0x3F: opCMC(); break;
            
            case 0x22: opSHLD(opcode[1], opcode[2]); break; 
            case 0x32: opSTA(opcode[1], opcode[2]);  break;
            case 0x2A: opLHLD(opcode[1], opcode[2]); break;
            case 0x3A: opLDA(opcode[1], opcode[2]);  break;

            case 0x02:
            case 0x12:
                opSTAX(_getRp(*opcode)); break;
            case 0x0A:
            case 0x1A:
                opLDAX(_getRp(*opcode)); break;

            default: goto not_implemented;
        }
    } else if (nibble0 >= 4 && nibble0 <= 7) {
        if (nibble1 == 6) { /* HLT */ }
        else
            opMOV(_getDest(*opcode), _getSrc(*opcode));
    } else if (nibble0 == 8) {
        if (nibble1 <= 7)
            opADD(_getSrc(*opcode));
        else
            opADC(_getSrc(*opcode));
    } else if (nibble0 == 9) {
        if (nibble1 <= 7)
            opSUB(_getSrc(*opcode));
        else
            opSBB(_getSrc(*opcode));
    } else if (nibble0 == 0xA) {
        if (nibble1 <= 7)
            opANA(_getSrc(*opcode));
        else
            opXRA(_getSrc(*opcode));
    } else if (nibble0 == 0xB) {
        if (nibble1 <= 7)
            opORA(_getSrc(*opcode));
        else
            opCMP(_getSrc(*opcode));
    } else {
        if (nibble1 == 2 || nibble1 == 0xA) {
            opJcondition(_getCond(*opcode), opcode[1], opcode[2]);
            return true;
        } else if (nibble1 == 0xD) {
            opCALL(opcode[1], opcode[2]);
            return true;
        } else if (nibble1 == 4 || nibble1 == 0xC) {
            opCcondition(_getCond(*opcode), opcode[1], opcode[2]);
            return true;
        } else {
            switch (*opcode) {
                case 0xC6: opADI(opcode[1]); break;
                case 0xCE: opACI(opcode[1]); break;
                case 0xD6: opSUI(opcode[1]); break;
                case 0xDE: opSBI(opcode[1]); break;
                case 0xE6: opANI(opcode[1]); break;
                case 0xEE: opXRI(opcode[1]); break;
                case 0xF6: opORI(opcode[1]); break;
                case 0xFE: opCPI(opcode[1]); break;

                case 0xEB: opXCHG(); break;

                case 0xC3:
                case 0xCB:
                    opJMP(opcode[1], opcode[2]); return true;

                default: goto not_implemented;
            }
        }
    }

    registers.pc++;
    return true;

    not_implemented: {
        printf("Instruction not implemented: 0x%02x!\n", *opcode);
        return false;
    }
}