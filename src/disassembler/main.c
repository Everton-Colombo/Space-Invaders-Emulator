#include <stdio.h>
#include <stdlib.h>
#include "8080_disassembler.h"

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