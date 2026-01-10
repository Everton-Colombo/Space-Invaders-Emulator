#ifndef _8080_DISASSEMBLER
#define _8080_DISASSEMBLER

#include <stdint.h>

int disassemble_next_8080(uint8_t* rom, unsigned int pc);

#endif