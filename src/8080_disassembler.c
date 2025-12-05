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
        case 0x00: printf("NOP\n"); return 1;

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


            LHLDadd r (Load H and L direct)

            (L) <- ((byte3)(byte2))
            (H) <- ((byte3)(byte2) + 1)
            The content of the memory location, whose address
            is specified in byte 2 and byte 3 of the instruction, is
            moved to register L. The content of the memory location at the succeeding address is moved to register H.

            0|0|1|0|1|0|1|0 + a16
            
        */
       case 0x22: printf("SHLD %02x%02x\n", op[2], op[1]); return 3;
       case 0x2A: printf("LHLD %02x%02x\n", op[2], op[1]); return 3;

        default: printf("UNKNOWN\n"); return 1;
    }
}