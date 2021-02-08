#ifndef _MIPS_FUNCTIONALITY_H_
#define _MIPS_FUNCTIONALITY_H_

#include <stdio.h>

/*
    [ Function Format of Instructions ]

R-type: 3 registers, 1 shift amount
    int (* funcptr)(int, int, int, int);

I-type: 2 registers, 1 immediate
    int (* funcptr)(int, int, int);

J-type: 1 target
    int (* funcptr)(int);
*/

// R-format Instructions
int slt(int rd, int rs, int rt, int sa);
int sltu(int rd, int rs, int rt, int sa);
int jr(int rd, int rs, int rt, int sa);

// I-format Instructions
int beq(int rs, int rt, int label);
int bne(int rs, int rt, int label);
int slti(int rs, int rt, int immedi);
int sltiu(int rs, int rt, int immedi);


// J-format Instructions
int j(int target);
int jal(int target);



#endif /*  _MIPS_FUNCTIONALITY_H */