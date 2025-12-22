#pragma once
#include <iostream>
#include <stdexcept>

struct ConditionFlags8080 {
    bool z  : 1;
    bool s  : 1;
    bool p  : 1;
    bool cy : 1;
    bool ac : 1;
    uint8_t _pad : 3;
};

enum class RegisterPairId8080 {
    BC,
    DE,
    HL,
    SP,
    PC
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
            case RegisterPairId8080::BC: return (static_cast<uint16_t>(b) << 8) | c;
            case RegisterPairId8080::DE: return (static_cast<uint16_t>(d) << 8) | e;
            case RegisterPairId8080::HL: return (static_cast<uint16_t>(h) << 8) | l;
            case RegisterPairId8080::SP: return sp;

            default: throw std::invalid_argument("Unsupported rp for getPair(): " + std::to_string(static_cast<int>(rp)));
        }
    }

    std::pair<uint8_t*, uint8_t*> getPairAddr(RegisterPairId8080 rp) {
        switch (rp) {
            case RegisterPairId8080::BC: return std::make_pair(&b, &c);
            case RegisterPairId8080::DE: return std::make_pair(&d, &e);
            case RegisterPairId8080::HL: return std::make_pair(&h, &l);

            default: throw std::invalid_argument("Unsupported rp for getPairAddr(): " + std::to_string(static_cast<int>(rp)));
        }
    }

    void setPair(RegisterPairId8080 rp, uint8_t dl, uint8_t dh) {
        switch (rp) {
            case RegisterPairId8080::BC:
                b = dh;
                c = dl;
                break;
            case RegisterPairId8080::DE:
                d = dh;
                e = dl;
                break;
            case RegisterPairId8080::HL: 
                h = dh;
                l = dl;
                break;
            case RegisterPairId8080::SP:
                sp = (static_cast<uint16_t>(dh) << 8) | dl;
                break;
            case RegisterPairId8080::PC:
                pc = (static_cast<uint16_t>(dh) << 8) | dl;
                break;
        }
    }

    void setPair(RegisterPairId8080 rp, uint16_t data) {
        setPair(rp, static_cast<uint8_t>(data & 0x00ff), static_cast<uint8_t>(data >> 8));
    }
};

enum class SrcDestId8080 {
    A = 0b111,
    B = 0b000,
    C,
    D,
    E,
    H,
    L,
    M
};

enum class ConditionId8080 {
    NZ,
    Z,
    NC,
    C,
    PO,
    PE,
    P,
    M
};

class CPU_8080 {
private:
    ConditionFlags8080 conditionFlags = {};
    Registers8080 registers = {};

    uint8_t* memory;
    bool interruptsEnabled = true;

    uint8_t* _getAddr(SrcDestId8080 id);
    void _setArithmeticConditionFlags(uint16_t operationResult);
    bool _evalCond(ConditionId8080 ccc);

    static inline SrcDestId8080 _getSrc(uint8_t op) {
        return static_cast<SrcDestId8080>(op & 0b00000111);
    }

    static inline SrcDestId8080 _getDest(uint8_t op) {
        return static_cast<SrcDestId8080>((op & 0b00111000) >> 3);
    }

    static inline RegisterPairId8080 _getRp(uint8_t op) {
        return static_cast<RegisterPairId8080>((op & 0b00110000) >> 4);
    }

    static inline ConditionId8080 _getCond(uint8_t op) {
        return static_cast<ConditionId8080>((op & 0b00111000) >> 3);
    }

    // Data Transfer Group:
    void opLXI(RegisterPairId8080 rp, uint8_t dl, uint8_t dh);
    void opMOV(SrcDestId8080 dest, SrcDestId8080 src);
    void opMVI(SrcDestId8080 dest, uint8_t data);
    void opLDA(uint8_t al, uint8_t ah);
    void opSTA(uint8_t al, uint8_t ah);
    void opLHLD(uint8_t al, uint8_t ah);
    void opSHLD(uint8_t al, uint8_t ah);
    void opLDAX(RegisterPairId8080 rp);
    void opSTAX(RegisterPairId8080 rp);
    void opXCHG();

    // Arithmetic Group:
    void opADD(SrcDestId8080 src);
    void opADI(uint8_t data);
    void opADC(SrcDestId8080 src);
    void opACI(uint8_t data);
    void opSUB(SrcDestId8080 src);
    void opSUI(uint8_t data);
    void opSBB(SrcDestId8080 src);
    void opSBI(uint8_t data);
    void opANA(SrcDestId8080 src);
    void opANI(uint8_t data);
    void opXRA(SrcDestId8080 src);
    void opXRI(uint8_t data);
    void opORA(SrcDestId8080 src);
    void opORI(uint8_t data);
    void opCMP(SrcDestId8080 src);
    void opCPI(uint8_t data);
    void opINX(RegisterPairId8080 rp);
    void opDCX(RegisterPairId8080 rp);
    void opINR(SrcDestId8080 dest);
    void opDCR(SrcDestId8080 dest);
    void opRLC();
    void opRRC();
    void opRAL();
    void opRAR();
    void opCMA();
    void opCMC();
    void opSTC();
    void opDAD(RegisterPairId8080 rp);
    void opDAA();

    // Branch Group:
    void opJMP(uint8_t al, uint8_t ah);
    void opJcondition(ConditionId8080 ccc, uint8_t al, uint8_t ah);
    void opCALL(uint8_t al, uint8_t ah);
    void opCcondition(ConditionId8080 ccc, uint8_t al, uint8_t ah);
    void opRET();
    void opRcondition(ConditionId8080 ccc);
    void opRST(uint8_t nnn);
    void opPCHL();

    // Stack, I/O, and Machine Control Group: 
    void opEI();
    void opDI();
    void opIN(uint8_t port);
    void opOUT(uint8_t port);
    void opPUSH(RegisterPairId8080 rp);
    void opPOP(RegisterPairId8080 rp);
    void opPUSHpsw();
    void opPOPpsw();
    void opXTHL();
    void opSPHL();

public:
    CPU_8080(uint8_t* memoryBaseAddress) {
        this->memory = memoryBaseAddress;
    }

    bool tick();

};