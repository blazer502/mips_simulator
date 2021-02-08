#ifndef _MIPS_UNIT_

#include <stdio.h>
#include <string.h>

#include "unit.h"

void __init_cpu(FILE *fp) {
    
    int i = 0;
    int fdata = 0;

    if (fp == NULL) return;

    // Initialize Register
    for (i = 0; i < 32; i++) {
        strcpy(cpu.registers[i].name, register_table[i]);
        // fscanf(fp, "%d", &fdata);
        cpu.registers[i].data = fdata;
    }

    // Initialize PC
    cpu.cycle = 0;
    cpu.counter = 0;
}

#endif /*  _MIPS_UNIT_ */