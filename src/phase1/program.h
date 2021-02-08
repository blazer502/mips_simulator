#ifndef _MIPS_PROGRAM_H_
#define _MIPS_PROGRAM_H_

#include <stdio.h>

//
//  Main Program
//
struct program {
    unsigned int cycle;
    unsigned int inst_count;

    char bin_name[30];
    FILE *bin_file;

    char reg_name[30];
    FILE *reg_file;
};



#endif /*  _MIPS_PROGRAM_H */