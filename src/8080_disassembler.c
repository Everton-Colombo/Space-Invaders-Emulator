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
    int opbytes = 1;

    printf("%04X\t");
    switch(*op) {
        case 0x00: // NOP
            printf("NOP\n");
            break;

        /*
            LXI rp, data16 (Load register pair immediate)

            (rh) <- (byte 3),
            (rl) <- (byte 2)
            Byte 3 of the instruction is moved into the high-order
            register (rh) of the register pair rp. Byte 2 of the instruction is moved into the low-order register (rl) of
            the register pair rp.

            0|0|R|P|0|0|0|1
        */
        case 0x01: // LXI B - Pair B & C
            printf("LXI B, %02x%02x\n", op[2], op[1]);
            opbytes = 3;
            break;
        case 0x11: // LXI D - Pair D & E
            printf("LXI D, %02x%02x\n", op[2], op[1]);
            opbytes = 3;
            break;
        case 0x21: // LXI H - Pair H & L
            printf("LXI H, %02x%02x\n", op[2], op[1]);
            opbytes = 3;
            break;
        case 0x31: // LXI SP
            printf("LXI SP, %02x%02x\n", op[2], op[1]);
            opbytes = 3;
            break;

        default:
            printf("UNKNOWN\n");
            break;
    }

    return opbytes;
}