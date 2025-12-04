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
        case 0x02: printf("STAX B"); return 1;
        case 0x12: printf("STAX D"); return 1;
        case 0x22: printf("STAX H"); return 1;
        case 0x32: printf("STAX SP"); return 1;

        /*
            INX rp (Increment register pair)

            (rh) (rl) <- (rh) (rl) + 1
            The content of the register pair rp is incremented byone.
            Note: No condition ftags are affected

            0|0|R|P|0|0|1|1
        */
        case 0x03: printf("INX B"); return 1;
        case 0x13: printf("INX D"); return 1;
        case 0x23: printf("INX H"); return 1;
        case 0x33: printf("INX SP"); return 1;

        default: printf("UNKNOWN\n"); return 1;
    }
}