#ifndef _MIPS_FUNCTIONALITY_

#include <stdint.h>
#include "functionality.h"
#include "unit.h"

// R-format Instructions
int slt(int rd, int rs, int rt, int sa)
{
    return 0;
}

int sltu(int rd, int rs, int rt, int sa)
{
    return 0;
}

int jr(int rd, int rs, int rt, int sa)
{
    // cpu.counter = rs;
    int sp = cpu.registers[29].data;
    cpu.counter = stack[sp/4] - 4;
    cpu.registers[29].data -= 4;
    return 1;
}


// I-format Instructions
int beq(int rs, int rt, int label)
{
    //if (rs == rt)
    cpu.counter += (int16_t)label * 4;
    return 1;
}

int bne(int rs, int rt, int label)
{
    //if (rs != rt)
    cpu.counter += (int16_t)label * 4;
    return 1;
}

int slti(int rs, int rt, int immedi)
{
    return 0;
}

int sltiu(int rs, int rt, int immedi)
{
    return 0;
}


// J-format Instructions
int j(int target)
{
    cpu.counter = (target << 2) - 4; 
    return 1;
}

int jal(int target)
{
    int sp = 0;
    // Store return address
    cpu.registers[29].data += 4;
    sp = cpu.registers[29].data;
    stack[sp/4] = cpu.counter + 4;
 
    // Set next instruction address
    cpu.counter = (target << 2) - 4;
    return 1;
}



#endif /*  _MIPS_FUNCTIONALITY_ */
