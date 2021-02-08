#ifndef _MIPS_UNIT_H_
#define _MIPS_UNIT_H_

#include <stdint.h>
#include <stdio.h>

//
// CPU
//
struct reg
{
    char name[6];
    int data;
};

static const char register_table[32][6] = {
    // Constant value 0
    "$zero",
    "$at",
    // Values for results and expression evaluation
    "$v0", "$v1",
    // Arguments
    "$a0", "$a1", "$a2", "$a3",
    // Temporaries
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    // Saved
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    // More Temporaries
    "$t8", "$t9",

    "$gp", // Global Pointer
    "$sp", // Stack Pointer
    "$fp", // Frame Pointer
    "$ra"  // Return Address
};

struct _cpu {
    unsigned int cycle;
    int counter;
    struct reg registers[32];
};

struct _cpu cpu;

//
// Memory
//      instruction memory(code) is located
uint8_t data[0xFFFFF];
int stack[0xFFFFF];


void __init_cpu(FILE *fopen);

#endif /*  _MIPS_UNIT_H */