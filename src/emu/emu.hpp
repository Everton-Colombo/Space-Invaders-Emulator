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
};

enum RegisterId8080 {
    A = 0b111,
    B = 0b000,
    C,
    D,
    E,
    H,
    L,
    M
};

class CPU_8080 {
private:
    ConditionFlags8080 conditionFlags;
    Registers8080 registers;

    uint8_t* memory;
    bool intEnable;

    uint8_t* _getByteAddr(RegisterId8080 id);

    void opMOV(RegisterId8080 dest, RegisterId8080 src);

public:
    CPU_8080(uint8_t* memoryBaseAddress) {
        this->memory = memoryBaseAddress;
    }

    bool tick();

};