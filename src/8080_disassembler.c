#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int disassemble_next_8080(uint8_t* rom, unsigned int pc);

int main(int argc, char** argv) {
    FILE* rom_fp = fopen(argv[1], "rb");
    if (rom_fp == NULL) {
        printf("ERROR: Could not open %s!\n", argv[1]);
        exit(1);
    }

    fseek(rom_fp, 0, SEEK_END);
    size_t file_size = ftell(rom_fp);
    uint8_t* rom = malloc(file_size * sizeof(uint8_t));

    fseek(rom_fp, 0, SEEK_SET);
    fread(rom, sizeof(uint8_t), file_size, rom_fp);
    fclose(rom_fp);

    unsigned int pc = 0;
    while (pc < file_size) {
        pc += disassemble_next_8080(rom, pc);
    }
    printf("\n\n");

    free(rom);
    return 0;
}


int disassemble_next_8080(uint8_t* rom, unsigned int pc) {
    uint8_t* op = &rom[pc];

    printf("%04X\t", pc);
    switch(*op) {
        case 0x00:
        case 0x10:
        case 0x20:
        case 0x30:
        case 0x08:
        case 0x18:
        case 0x28:
        case 0x38:
            printf("NOP\n"); return 1;

        /*
            LXI rp, data16 (Load register pair immediate)

            (rh) <- (byte 3),
            (rl) <- (byte 2)
            Byte 3 of the instruction is moved into the high-order
            register (rh) of the register pair rp. Byte 2 of the instruction is moved into the low-order register (rl) of
            the register pair rp.

            0|0|R|P|0|0|0|1
        */
        case 0x01: printf("LXI B, 0x%02x%02x\n", op[2], op[1]); return 3;
        case 0x11: printf("LXI D, 0x%02x%02x\n", op[2], op[1]); return 3;
        case 0x21: printf("LXI H, 0x%02x%02x\n", op[2], op[1]); return 3;
        case 0x31: printf("LXI SP, 0x%02x%02x\n", op[2], op[1]); return 3;
        
        /*
            STAX rp (Store accumulator indirect)

            ((rp)) <- (A)
            The content of register A is moved to the memory location whose address is in the register pair rp. Note:
            only register pairs rp=B (registers B and C) or rp=D
            (registers D and E) may be specified.

            0|0|R|P|0|0|1|0
        */
        case 0x02: printf("STAX B\n"); return 1;
        case 0x12: printf("STAX D\n"); return 1;

        /*
            INX rp (Increment register pair)

            (rh) (rl) <- (rh) (rl) + 1
            The content of the register pair rp is incremented byone.
            Note: No condition ftags are affected

            0|0|R|P|0|0|1|1
        */
        case 0x03: printf("INX B\n"); return 1;
        case 0x13: printf("INX D\n"); return 1;
        case 0x23: printf("INX H\n"); return 1;
        case 0x33: printf("INX SP\n"); return 1;

        // DCX rp (Decrement register pair)
        case 0x0B: printf("dcX B\n"); return 1;
        case 0x1B: printf("dcX D\n"); return 1;
        case 0x2B: printf("dcX H\n"); return 1;
        case 0x3B: printf("dcX SP\n"); return 1;

        /*
            INR r (Increment Register) | m (Increment memory)

            (r) <- (r) + 1 | ((H) (L)) <- ((H) (L)) + 1
            The content of register r is incremented by one.
            or
            The content of the memory location whose address
            is contained in the H and L registers is incremented
            by one
            Note: All condition flags except CY are affected.

            0|0|D|D|D|1|0|0
            0|0|1|1|0|1|0|0
        */
        case 0x04: printf("INR B\n"); return 1;
        case 0x14: printf("INR D\n"); return 1;
        case 0x24: printf("INR H\n"); return 1;
        case 0x34: printf("INR M\n"); return 1;
        case 0x0C: printf("INR C\n"); return 1;
        case 0x1C: printf("INR E\n"); return 1;
        case 0x2C: printf("INR L\n"); return 1;
        case 0x3C: printf("INR A\n"); return 1;

        /*
            DCR r (Decrement Register) | m (Decrement memory)

            (r) <- (r) - 1 | ((H) (L)) <- ((H) (L)) - 1
            The content of register r is decremented by one.
            or
            The content of the memory location whose address
            is contained in the H and L registers is decremented
            by one
            Note: All condition flags except CY are affected.

            0|0|D|D|D|1|0|1
            0|0|1|1|0|1|0|1
        */
        case 0x05: printf("DCR B\n"); return 1;
        case 0x15: printf("DCR D\n"); return 1;
        case 0x25: printf("DCR H\n"); return 1;
        case 0x35: printf("DCR M\n"); return 1;
        case 0x0D: printf("DCR C\n"); return 1;
        case 0x1D: printf("DCR E\n"); return 1;
        case 0x2D: printf("DCR L\n"); return 1;
        case 0x3D: printf("DCR A\n"); return 1;

        /*
            MVI r (Move immediate) | m (Move to memory immediate)

            (r) <- (byte 2) | ((H) (L)) <- (byte 2)
            The content of byte 2 of the instruction is moved to
            registerr. 

            0|0|D|D|D|1|1|0
            
        */
        case 0x06: printf("MVI B, 0x%02x\n", op[1]); return 2;
        case 0x16: printf("MVI D, 0x%02x\n", op[1]); return 2;
        case 0x26: printf("MVI H, 0x%02x\n", op[1]); return 2;
        case 0x36: printf("MVI M, 0x%02x\n", op[1]); return 2;
        case 0x0E: printf("MVI C, 0x%02x\n", op[1]); return 2;
        case 0x1E: printf("MVI E, 0x%02x\n", op[1]); return 2;
        case 0x2E: printf("MVI L, 0x%02x\n", op[1]); return 2;
        case 0x3E: printf("MVI A, 0x%02x\n", op[1]); return 2;

        /*
            SHLD addr (Store H and L direct)

            ((byte3)(byte2)) <- (L)
            ((byte 3)(byte 2) + 1) <- (H)
            The content of register L is moved to the memory location whose address is specified in byte 2 and byte
            3. The content of register H is moved to the succeeding memory location.

            0|0|1|0|0|0|1|0 + a16


            LHLD addr (Load H and L direct)

            (L) <- ((byte3)(byte2))
            (H) <- ((byte3)(byte2) + 1)
            The content of the memory location, whose address
            is specified in byte 2 and byte 3 of the instruction, is
            moved to register L. The content of the memory location at the succeeding address is moved to register H.

            0|0|1|0|1|0|1|0 + a16
            
        */
        case 0x22: printf("SHLD 0x%02x%02x\n", op[2], op[1]); return 3;
        case 0x2A: printf("LHLD 0x%02x%02x\n", op[2], op[1]); return 3;

        /*
            STA addr (Store Accumulator direct)

            ((byte3)(byte2)) <- (A)
            The content of the accumulator is moved to the
            memory location whose address is specified in byte
            2 and byte 3 of the instruction. 

            0011|0010 + a16
        */
        case 0x32: printf("STA 0x%02x%02x\n", op[2], op[1]); return 3;

        /*
            RLC (Rotate left)

            (A_n+1) <- (A_n) ; (A_0) <- (A_7)
            (CY) <- (A_7)
            The content of the accumulator is rotated left one
            position. The low order bit and the CY flag are both
            set to the value shifted out of the high order bit position. Only the CY flag is affected.
            
            0000|0111
        */
        case 0x07: printf("RLC\n"); return 1;

       /*
            RRC (Rotate right)

            (A_n) <- (A_n-1) ; (A_7) <- (A_0)
            (CY) <- (A_0)
            The content of the accumulator is rotated right one
            position. The high order bit and the CY flag are both
            set to the value shifted out of the low order bit position. Only the CY flag is affected.
            
            0000|1111
        */
        case 0x0F: printf("RRC\n"); return 1;

        case 0x17: printf("RAL\n"); return 1;
        case 0x1F: printf("RAR\n"); return 1;

        case 0x27: printf("DAA\n"); return 1;

        case 0x37: printf("STC\n"); return 1;

        // DAD rp: (Add register pair to H and L)
        case 0x09: printf("DAD B\n"); return 1;
        case 0x19: printf("DAD D\n"); return 1;
        case 0x29: printf("DAD H\n"); return 1;
        case 0x39: printf("DAD SP\n"); return 1;

        // LDAX rp: Load accumulator indirect
        case 0x0A: printf("LDAX B\n"); return 1;
        case 0x1A: printf("LDAX D\n"); return 1;

        // LDA addr: Load accumulator direct
        case 0x3A: printf("LDA 0x%02x%02x\n", op[2], op[1]); return 3;

        // CMA: Complement A
        case 0x2F: printf("CMA\n"); return 1;

        // CMC: Complement Carry
        case 0x3F: printf("CMC\n"); return 1;

        /*
            MOV r1, r2 (Move Register)
            (r1) <- (r2)
            The content of register r2 is moved to register r1.

            0|1|D|D|D|S|S|S
        */
        case 0x40: printf("MOV B,B\n"); return 1;
        case 0x41: printf("MOV B,C\n"); return 1;
        case 0x42: printf("MOV B,D\n"); return 1;
        case 0x43: printf("MOV B,E\n"); return 1;
        case 0x44: printf("MOV B,H\n"); return 1;
        case 0x45: printf("MOV B,L\n"); return 1;
        case 0x46: printf("MOV B,M\n"); return 1;
        case 0x47: printf("MOV B,A\n"); return 1;
        case 0x48: printf("MOV C,B\n"); return 1;
        case 0x49: printf("MOV C,C\n"); return 1;
        case 0x4A: printf("MOV C,D\n"); return 1;
        case 0x4B: printf("MOV C,E\n"); return 1;
        case 0x4C: printf("MOV C,H\n"); return 1;
        case 0x4D: printf("MOV C,L\n"); return 1;
        case 0x4E: printf("MOV C,M\n"); return 1;
        case 0x4F: printf("MOV C,A\n"); return 1;

        case 0x50: printf("MOV D,B\n"); return 1;
        case 0x51: printf("MOV D,C\n"); return 1;
        case 0x52: printf("MOV D,D\n"); return 1;
        case 0x53: printf("MOV D,E\n"); return 1;
        case 0x54: printf("MOV D,H\n"); return 1;
        case 0x55: printf("MOV D,L\n"); return 1;
        case 0x56: printf("MOV D,M\n"); return 1;
        case 0x57: printf("MOV D,A\n"); return 1;
        case 0x58: printf("MOV E,B\n"); return 1;
        case 0x59: printf("MOV E,C\n"); return 1;
        case 0x5A: printf("MOV E,D\n"); return 1;
        case 0x5B: printf("MOV E,E\n"); return 1;
        case 0x5C: printf("MOV E,H\n"); return 1;
        case 0x5D: printf("MOV E,L\n"); return 1;
        case 0x5E: printf("MOV E,M\n"); return 1;
        case 0x5F: printf("MOV E,A\n"); return 1;

        case 0x60: printf("MOV H,B\n"); return 1;
        case 0x61: printf("MOV H,C\n"); return 1;
        case 0x62: printf("MOV H,D\n"); return 1;
        case 0x63: printf("MOV H,E\n"); return 1;
        case 0x64: printf("MOV H,H\n"); return 1;
        case 0x65: printf("MOV H,L\n"); return 1;
        case 0x66: printf("MOV H,M\n"); return 1;
        case 0x67: printf("MOV H,A\n"); return 1;
        case 0x68: printf("MOV L,B\n"); return 1;
        case 0x69: printf("MOV L,C\n"); return 1;
        case 0x6A: printf("MOV L,D\n"); return 1;
        case 0x6B: printf("MOV L,E\n"); return 1;
        case 0x6C: printf("MOV L,H\n"); return 1;
        case 0x6D: printf("MOV L,L\n"); return 1;
        case 0x6E: printf("MOV L,M\n"); return 1;
        case 0x6F: printf("MOV L,A\n"); return 1;

        case 0x70: printf("MOV M,B\n"); return 1;
        case 0x71: printf("MOV M,C\n"); return 1;
        case 0x72: printf("MOV M,D\n"); return 1;
        case 0x73: printf("MOV M,E\n"); return 1;
        case 0x74: printf("MOV M,H\n"); return 1;
        case 0x75: printf("MOV M,L\n"); return 1;
        case 0x77: printf("MOV M,A\n"); return 1;
        case 0x78: printf("MOV A,B\n"); return 1;
        case 0x79: printf("MOV A,C\n"); return 1;
        case 0x7A: printf("MOV A,D\n"); return 1;
        case 0x7B: printf("MOV A,E\n"); return 1;
        case 0x7C: printf("MOV A,H\n"); return 1;
        case 0x7D: printf("MOV A,L\n"); return 1;
        case 0x7E: printf("MOV A,M\n"); return 1;
        case 0x7F: printf("MOV A,A\n"); return 1;

        // HLT: Halt
        case 0x76: printf("HLT\n"); return 1;

        /*
            ADD r (Add register) | M (Add memory)

            (A) <- (A) + (r) | (A) <- (A) + ((H) (L))
            The content of register r is added to the content of the
            accumulator. The result is placed in the accumulator.
            Note: All condition flags are affected.

            1|0|0|0|0|S|S|S
        */
        case 0x80: printf("ADD B\n"); return 1;
        case 0x81: printf("ADD C\n"); return 1;
        case 0x82: printf("ADD D\n"); return 1;
        case 0x83: printf("ADD E\n"); return 1;
        case 0x84: printf("ADD H\n"); return 1;
        case 0x85: printf("ADD L\n"); return 1;
        case 0x86: printf("ADD M\n"); return 1;
        case 0x87: printf("ADD A\n"); return 1;

        /*
            ADC r (Add register with carry) | M (Add memory with carry)

            (A) <- (A) + (r) + (CY) | (A) <- (A) + ((H) (L)) + (CY)
            The content of register r and the content of the CY flag
            are added to the content of the accumulator. The result
            is placed in the accumulator.
            Note: All condition flags are affected.

            1|0|0|0|1|S|S|S
        */
        case 0x88: printf("ADC B\n"); return 1;
        case 0x89: printf("ADC C\n"); return 1;
        case 0x8A: printf("ADC D\n"); return 1;
        case 0x8B: printf("ADC E\n"); return 1;
        case 0x8C: printf("ADC H\n"); return 1;
        case 0x8D: printf("ADC L\n"); return 1;
        case 0x8E: printf("ADC M\n"); return 1;
        case 0x8F: printf("ADC A\n"); return 1;

        /*
            SUB r (Subtract register) | M (Subtract memory)

            (A) <- (A) - (r) | (A) <- (A) - ((H) (L))
            The content of register r is subtracted from the content of the accumulator. The result is placed in the
            accumulator.
            Note: All condition flags are affected.

            1|0|0|1|0|S|S|S
        */
        case 0x90: printf("SUB B\n"); return 1;
        case 0x91: printf("SUB C\n"); return 1;
        case 0x92: printf("SUB D\n"); return 1;
        case 0x93: printf("SUB E\n"); return 1;
        case 0x94: printf("SUB H\n"); return 1;
        case 0x95: printf("SUB L\n"); return 1;
        case 0x96: printf("SUB M\n"); return 1;
        case 0x97: printf("SUB A\n"); return 1;

        /*
            SBB r (Subtract register with borrow) | M (Subtract memory with borrow)

            (A) <- (A) - (r) - (CY) | (A) <- (A) - ((H) (L)) - (CY)
            The content of register r and the content of the CY flag
            are both subtracted from the accumulator. The result is
            placed in the accumulator.
            Note: All condition flags are affected.

            1|0|0|1|1|S|S|S
        */
        case 0x98: printf("SBB B\n"); return 1;
        case 0x99: printf("SBB C\n"); return 1;
        case 0x9A: printf("SBB D\n"); return 1;
        case 0x9B: printf("SBB E\n"); return 1;
        case 0x9C: printf("SBB H\n"); return 1;
        case 0x9D: printf("SBB L\n"); return 1;
        case 0x9E: printf("SBB M\n"); return 1;
        case 0x9F: printf("SBB A\n"); return 1;

        /*
            ANA r (Logical and register) | M (Logical and memory)

            (A) <- (A) ^ (r) | (A) <- (A) ^ ((H) (L))
            The content of register r is logically anded with the
            content of the accumulator. The result is placed in the
            accumulator. The CY flag is cleared.
            Note: The S, Z, P are affected.

            1|0|1|0|0|S|S|S
        */
        case 0xA0: printf("ANA B\n"); return 1;
        case 0xA1: printf("ANA C\n"); return 1;
        case 0xA2: printf("ANA D\n"); return 1;
        case 0xA3: printf("ANA E\n"); return 1;
        case 0xA4: printf("ANA H\n"); return 1;
        case 0xA5: printf("ANA L\n"); return 1;
        case 0xA6: printf("ANA M\n"); return 1;
        case 0xA7: printf("ANA A\n"); return 1;

        /*
            XRA r (Exclusive OR register) | M (Exclusive OR memory)

            (A) <- (A) XOR (r) | (A) <- (A) XOR ((H) (L))
            The content of register r is exclusive-or'd with the
            content of the accumulator. The result is placed in the
            accumulator. The CY and AC flags are cleared.
            Note: The S, Z, P are affected.

            1|0|1|0|1|S|S|S
        */
        case 0xA8: printf("XRA B\n"); return 1;
        case 0xA9: printf("XRA C\n"); return 1;
        case 0xAA: printf("XRA D\n"); return 1;
        case 0xAB: printf("XRA E\n"); return 1;
        case 0xAC: printf("XRA H\n"); return 1;
        case 0xAD: printf("XRA L\n"); return 1;
        case 0xAE: printf("XRA M\n"); return 1;
        case 0xAF: printf("XRA A\n"); return 1;

        /*
            ORA r (Logical OR register) | M (Logical OR memory)

            (A) <- (A) V (r) | (A) <- (A) V ((H) (L))
            The content of register r is inclusive-or'd with the
            content of the accumulator. The result is placed in the
            accumulator. The CY and AC flags are cleared.
            Note: The S, Z, P are affected.

            1|0|1|1|0|S|S|S
        */
        case 0xB0: printf("ORA B\n"); return 1;
        case 0xB1: printf("ORA C\n"); return 1;
        case 0xB2: printf("ORA D\n"); return 1;
        case 0xB3: printf("ORA E\n"); return 1;
        case 0xB4: printf("ORA H\n"); return 1;
        case 0xB5: printf("ORA L\n"); return 1;
        case 0xB6: printf("ORA M\n"); return 1;
        case 0xB7: printf("ORA A\n"); return 1;

        /*
            CMP r (Compare register) | M (Compare memory)

            (A) - (r) | (A) - ((H) (L))
            The content of register r is subtracted from the accumulator. The accumulator remains unchanged. The
            condition flags are set as a result of the subtraction.
            The Z flag is set to 1 if (A) = (r). The CY flag is set to 1
            if (A) < (r).

            1|0|1|1|1|S|S|S
        */
        case 0xB8: printf("CMP B\n"); return 1;
        case 0xB9: printf("CMP C\n"); return 1;
        case 0xBA: printf("CMP D\n"); return 1;
        case 0xBB: printf("CMP E\n"); return 1;
        case 0xBC: printf("CMP H\n"); return 1;
        case 0xBD: printf("CMP L\n"); return 1;
        case 0xBE: printf("CMP M\n"); return 1;
        case 0xBF: printf("CMP A\n"); return 1;

        // Immediate arithmetic operations
        case 0xC6: printf("ADI 0x%02x\n", op[1]); return 2;
        case 0xCE: printf("ACI 0x%02x\n", op[1]); return 2;
        case 0xD6: printf("SUI 0x%02x\n", op[1]); return 2;
        case 0xDE: printf("SBI 0x%02x\n", op[1]); return 2;
        case 0xE6: printf("ANI 0x%02x\n", op[1]); return 2;
        case 0xEE: printf("XRI 0x%02x\n", op[1]); return 2;
        case 0xF6: printf("ORI 0x%02x\n", op[1]); return 2;
        case 0xFE: printf("CPI 0x%02x\n", op[1]); return 2;

        case 0XC0: printf("RNZ\n"); return 1; // Return if not zero
        case 0XD0: printf("RNC\n"); return 1; // Return on no carry
        case 0xE0: printf("RPO\n"); return 1; // Return on parity odd
        case 0xF0: printf("RP\n"); return 1;  // Return on positive
        case 0xC8: printf("RZ\n"); return 1;  // Return on zero
        case 0xD8: printf("RC\n"); return 1;  // Return on carry
        case 0xE8: printf("RPE\n"); return 1; // Return on parity even
        case 0xF8: printf("RM\n"); return 1;  // Return on minus
        case 0xC9:
        case 0xD9:
            printf("RET\n"); return 1; // Return
        
        /*
            POP rp (Pop register pair)

            (rl) <- ((SP))
            (rh) <- ((SP) + 1)
            (SP) <- (SP) + 2
            The content of the memory location, whose address
            is specified by the content of register SP, is moved to
            the low-order register of register pair rp. The content
            of the memory location, whose address is one more
            than the content of register SP, is moved to the highorder register of register pair rp. The content of register
            SP is incremented by 2. Note: Register pair rp = SP
            may not be specified.

            1|1|R|P|0|0|0|1
        */
        case 0xC1: printf("POP B\n"); return 1;
        case 0xD1: printf("POP D\n"); return 1;
        case 0xE1: printf("POP H\n"); return 1;
        case 0xF1: printf("POP PSW\n"); return 1;

        /*
            PUSH rp (Push register pair)

            ((SP) - 1) <- (rh)
            ((SP) - 2) <- (rl)
            (SP) <- (SP) - 2
            The content of the high-order register of register pair
            rp is moved to the memory location whose address is
            one less than the content of register SP. The content
            of the low-order register of register pair rp is moved
            to the memory location whose address is two less
            than the content of register SP. The content of register
            SP is decremented by 2. Note: Register pair rp = SP
            may not be specified.

            1|1|R|P|0|1|0|1
        */
        case 0xC5: printf("PUSH B\n"); return 1;
        case 0xD5: printf("PUSH D\n"); return 1;
        case 0xE5: printf("PUSH H\n"); return 1;
        case 0xF5: printf("PUSH PSW\n"); return 1;

        // JNZ a16: Jump if not zero
        case 0xC2: printf("JNZ 0x%02x%02x\n", op[2], op[1]); return 3;
        // JMP a16: Unconditional jump
        case 0xC3: 
        case 0xCB: 
            printf("JMP 0x%02x%02x\n", op[2], op[1]); return 3;
        // JNC a16: Jump if no carry
        case 0xD2: printf("JNC 0x%02x%02x\n", op[2], op[1]); return 3;
        // JPO a16: Jump if parity odd
        case 0xE2: printf("JPO 0x%02x%02x\n", op[2], op[1]); return 3;
        // JP a16: Jump if positive
        case 0xF2: printf("JP 0x%02x%02x\n", op[2], op[1]); return 3;
        // JZ a16: Jump if zero
        case 0xCA: printf("JZ 0x%02x%02x\n", op[2], op[1]); return 3;
        // JC a16: Jump if carry
        case 0xDA: printf("JC 0x%02x%02x\n", op[2], op[1]); return 3;
        // JPE a16: Jump if parity even
        case 0xEA: printf("JPE 0x%02x%02x\n", op[2], op[1]); return 3;
        // JM a16: Jump if minus
        case 0xFA: printf("JM 0x%02x%02x\n", op[2], op[1]); return 3;
        
        // PCHL: Jump to address in H:L
        case 0xE9: printf("PCHL\n"); return 1;
        // SPHL: (sp) <- (H) (L)
        case 0xF9: printf("SPHL\n"); return 1;

        // CNZ a16: Call if not zero
        case 0xC4: printf("CNZ 0x%02x%02x\n", op[2], op[1]); return 3;
        // CALL a16: Unconditional call
        case 0xCD: 
        case 0xDD: 
        case 0xED: 
        case 0xFD: 
            printf("CALL 0x%02x%02x\n", op[2], op[1]); return 3;
        // CNC a16: Call if no carry
        case 0xD4: printf("CNC 0x%02x%02x\n", op[2], op[1]); return 3;
        // CPO a16: Call if parity odd
        case 0xE4: printf("CPO 0x%02x%02x\n", op[2], op[1]); return 3;
        // CP a16: Call if positive
        case 0xF4: printf("CP 0x%02x%02x\n", op[2], op[1]); return 3;
        // CZ a16: Call if zero
        case 0xCC: printf("CZ 0x%02x%02x\n", op[2], op[1]); return 3;
        // CC a16: Call if carry
        case 0xDC: printf("CC 0x%02x%02x\n", op[2], op[1]); return 3;
        // CPE a16: Call if parity even
        case 0xEC: printf("CPE 0x%02x%02x\n", op[2], op[1]); return 3;
        // CM a16: Call if minus
        case 0xFC: printf("CM 0x%02x%02x\n", op[2], op[1]); return 3;

        // RST 0
        case 0xC7: printf("RST 0\n"); return 1;
        // RST 1
        case 0xCF: printf("RST 1\n"); return 1;
        // RST 2
        case 0xD7: printf("RST 2\n"); return 1;
        // RST 3
        case 0xDF: printf("RST 3\n"); return 1;
        // RST 4
        case 0xE7: printf("RST 4\n"); return 1;
        // RST 5
        case 0xEF: printf("RST 5\n"); return 1;
        // RST 6
        case 0xF7: printf("RST 6\n"); return 1;
        // RST 7
        case 0xFF: printf("RST 7\n"); return 1;

        // OUT d8
        case 0xD3: printf("OUT 0x%02x\n", op[1]); return 2;
        // IN d8
        case 0xDB: printf("IN 0x%02x\n", op[1]); return 2;

        // XTHL: Exchange stack top with H:L
        case 0xE3: printf("XTHL\n"); return 1;
        // XCHG: Exchange D:E with H:L
        case 0xEB: printf("XCHG\n"); return 1;

        // DI: Disable interrupts
        case 0xF3: printf("DI\n"); return 1;
        // EI: Enable interrupts
        case 0xFB: printf("EI\n"); return 1;
        
        default: printf("UNKNOWN\n"); return 1;
    }
}