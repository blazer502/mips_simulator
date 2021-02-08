#ifndef _MIPS_INSTRUCTION_
#define _MIPS_INSTRUCTION_

#include <stdint.h>
#include <stdio.h>

#include "unit.h"
#include "instruction.h"

extern struct _cpu cpu;
extern struct _control controler;
extern uint8_t data[];
extern uint32_t stack[]; // for branch instruction
extern struct instruction code[]; // instruction database

// Instruction Dictionary
struct R_field R_table[11] = {
    {"nop",   0, 0, 0, 0b0, ANON, 0},

    // arithmetic
    {"add",   0, 0, 0, 0b100000, RD|RS|RT, _R|_A},
    {"sub",   0, 0, 0, 0b100010, RD|RS|RT, _R|_A},

    // logical
    {"and",   0, 0, 0, 0b100100, RD|RS|RT, _R|_L},
    {"or",    0, 0, 0, 0b100101, RD|RS|RT, _R|_L},
    {"nor",   0, 0, 0, 0b100111, RD|RS|RT, _R|_L},
    {"sll",   0, 0, 0, 0b000000, RD|RT|SA, _R|_L},
    {"srl",   0, 0, 0, 0b000010, RD|RT|SA, _R|_L},

    // conditional branch
    {"slt",   0, 0, 0, 0b101010, RD|RS|RT, _R|_B},
    {"sltu",  0, 0, 0, 0b101011, RD|RS|RT, _R|_B},

    // unconditional branch
    {"jr",    0, 0, 0, 0b001000, RS, _R|_J}
};

struct I_field I_table[17] = {
    {"nop",   0, 0, 0b0, ANON, 0},
    // arithmetic
    {"addi", 0, 0, 0b001000, RT | RS | IMMED | REVER, _I|_A},

    // data transfer
    {"lw",  0, 0, 0b100011, RT | RS | IMMED, _I|_D},
    {"sw",  0, 0, 0b101011, RT | RS | IMMED, _I|_D},
    {"lh",  0, 0, 0b100001, RT | RS | IMMED, _I|_D},
    {"lhu", 0, 0, 0b100101, RT | RS | IMMED, _I|_D},
    {"sh",  0, 0, 0b101001, RT | RS | IMMED, _I|_D},
    {"lb",  0, 0, 0b100000, RT | RS | IMMED, _I|_D},
    {"lbu", 0, 0, 0b100100, RT | RS | IMMED, _I|_D},
    {"sb",  0, 0, 0b101000, RT | RS | IMMED, _I|_D},
    {"lui", 0, 0, 0b001111, RT | IMMED, _I|_D},

    // logical
    {"andi", 0, 0, 0b001100, RT | RS | IMMED | REVER, _I|_L},
    {"ori", 0, 0, 0b001101, RT | RS | IMMED | REVER, _I|_L},

    // conditional branch
    {"beq", 0, 0, 0b000100, RT | RS | LABEL, _I|_B},
    {"bne", 0, 0, 0b000101, RT | RS | LABEL, _I|_B},
    {"slti", 0, 0, 0b001010, RT | RS | IMMED | REVER, _I|_B},
    {"sltiu", 0, 0, 0b001011, RT | RS | IMMED | REVER, _I|_B}
};

struct J_field J_table[3] = {
    {"nop",   0, 0b0, ANON, 0},
    // unconditional branch
    {"j",   0, 0b000010, LABEL, _J},
    // jump to address of label and store retured address in $31
    {"jal", 0, 0b000011, LABEL, _J}
};

struct R_field *inst_r_find(uint32_t data)
{
    // All r-field instruction opcode is zero
    int i, funct;

    if (data == 0) return &(R_table[0]);

    funct = R_FUNC(data);
    
    for (i = 0; i < 11; i++) {
        if (R_table[i].funct == funct &&
            R_table[i].type != 0) {
            return &(R_table[i]);
        }
    }

    // nop
    return &(R_table[0]); 
}

struct I_field *inst_i_find(uint32_t data)
{
    int i, opcode;

    if (data == 0) return &(I_table[0]);

    opcode = I_OP(data);
    for (i = 0; i < 17; i++) {
        if (I_table[i].opcode == opcode &&
            I_table[i].type != 0) {
            return &(I_table[i]);
        }
    }
    
    // nop
    return &(I_table[0]);
}

struct J_field *inst_j_find(uint32_t data)
{
    int i, opcode;

    if (data == 0) return &(J_table[0]);

    opcode = J_OP(data);
    for (i = 0; i < 3; i++) {
        if (J_table[i].opcode == opcode &&
            J_table[i].type != 0) {
            return &(J_table[i]);
        }
    }

    // nop
    return &(J_table[0]);
}

#endif /*  _MIPS_INSTRUCTION_ */
