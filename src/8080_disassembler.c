#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

    for (int i = 0; i < file_size; i++) {
        if (i % 16 == 0)
            printf("\n");
        printf("%02x ", rom[i]);
    }
    printf("\n\n");

    free(rom);
    return 0;
}


int disassemble_next_8080(uint8_t* rom, int pc) {
    uint8_t* op = &rom[pc];

    printf("%04X\t");
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
        case 0x01: printf("LXI B, %02x%02x\n", op[2], op[1]); return 3;
        case 0x11: printf("LXI D, %02x%02x\n", op[2], op[1]); return 3;
        case 0x21: printf("LXI H, %02x%02x\n", op[2], op[1]); return 3;
        case 0x31: printf("LXI SP, %02x%02x\n", op[2], op[1]); return 3;
        
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
        case 0x06: printf("MVI B, %02x\n", op[1]); return 2;
        case 0x16: printf("MVI D, %02x\n", op[1]); return 2;
        case 0x26: printf("MVI H, %02x\n", op[1]); return 2;
        case 0x36: printf("MVI M, %02x\n", op[1]); return 2;
        case 0x0E: printf("MVI C, %02x\n", op[1]); return 2;
        case 0x1E: printf("MVI E, %02x\n", op[1]); return 2;
        case 0x2E: printf("MVI L, %02x\n", op[1]); return 2;
        case 0x3E: printf("MVI A, %02x\n", op[1]); return 2;

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
        case 0x22: printf("SHLD %02x%02x\n", op[2], op[1]); return 3;
        case 0x2A: printf("LHLD %02x%02x\n", op[2], op[1]); return 3;

        /*
            STA addr (Store Accumulator direct)

            ((byte3)(byte2)) <- (A)
            The content of the accumulator is moved to the
            memory location whose address is specified in byte
            2 and byte 3 of the instruction. 

            0011|0010 + a16
        */
        case 0x32: printf("STA %02x%02x\n", op[2], op[1]); return 3;

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
            RLC (Rotate right)

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
        case 0x3A: printf("LDA %02x%02x\n", op[2], op[1]); return 3;


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
        
        default: printf("UNKNOWN\n"); return 1;
    }
}