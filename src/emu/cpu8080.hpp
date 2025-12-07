#pragma once
#include <iostream>

struct ConditionFlags8080 {
    bool z  : 1;
    bool s  : 1;
    bool p  : 1;
    bool cy : 1;
    bool ac : 1;
    uint8_t _pad : 3;
};

struct Registers8080 {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp;
    uint16_t pc;

    uint16_t getPair(RegisterPairId8080 rp) const {
        switch (rp) {
            case BC: return (static_cast<uint16_t>(b) << 8) | c;
            case DE: return (static_cast<uint16_t>(d) << 8) | e;
            case HL: return (static_cast<uint16_t>(h) << 8) | l;
            case SP: return sp;
        }
    }

    void setPair(RegisterPairId8080 rp, uint8_t dl, uint8_t dh) {
        switch (rp) {
            case BC:
                b = dh;
                c = dl;
                break;
            case DE:
                d = dh;
                e = dl;
                break;
            case HL: 
                h = dh;
                l = dl;
                break;
            case SP:
                sp = (static_cast<uint16_t>(dh) << 8) | dl;
                break;
        }
    }
};

enum SrcDestId8080 {
    A = 0b111,
    B = 0b000,
    C,
    D,
    E,
    H,
    L,
    M
};

enum RegisterPairId8080 {
    BC,
    DE,
    HL,
    SP
};

class CPU_8080 {
private:
    ConditionFlags8080 conditionFlags;
    Registers8080 registers;

    uint8_t* memory;
    bool intEnable;

    uint8_t* _getAddr(SrcDestId8080 id);

    void opLXI(RegisterPairId8080 rp, uint8_t dl, uint8_t dh);
    void opMOV(SrcDestId8080 dest, SrcDestId8080 src);
    void opMVI(SrcDestId8080 dest, uint8_t data);

public:
    CPU_8080(uint8_t* memoryBaseAddress) {
        this->memory = memoryBaseAddress;
    }

    bool tick();

};