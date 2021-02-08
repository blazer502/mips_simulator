#ifndef _MIPS_INSTRUCTION_H_
#define _MIPS_INSTRUCTION_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "program.h"

#define REG_BITS 0x1F
#define ARITH_BITS 0x3F
#define CONST_BITS 0xFFFF

/*
    [ MIPS Instruction Types ]
    | Type  |31                  format                      0|
    |   R   | opcode(6) | rs(5) | rt(5) | shamt(5) | funct(6) |
    |   I   | opcode(6) | rs(5) | rt(5) |    Immediate(16)    |
    |   J   | opcode(6) |           address(26)               |

    R-Type:
        Opcode 000 000
        Main processor insturctions that do not require a target address,
        immediate value, or branch displacement use a R-type coding format.

        This format has fields for specifying of up to three registers and
        a shift amount.

        For instructions that do not use all of these fields, the unused
        fields are coded wiht all 0 bits.

        All R-type instructions use a 000 000 opcode.
        
        The operation is specified by function field.
*/
// R-format macro
#define R_OP(I) ((I >> 26) & ARITH_BITS)
#define R_RS(I) ((I >> 21) & REG_BITS)
#define R_RT(I) ((I >> 16) & REG_BITS)
#define R_RD(I) ((I >> 11) & REG_BITS)
#define R_SA(I) ((I >> 6)  & REG_BITS)
#define R_FUNC(I) (I & ARITH_BITS)
#define R_TYPE(I) (0b000000 | R_OP(I)) ? false : true

struct R_field {
    char op_name[8];
    // Register
    uint8_t rd;
    uint8_t rs;
    uint8_t rt;

    uint8_t funct;

    uint8_t flag;

    int (* funcptr)(int, int, int, int);
    
    uint8_t opcode;
    uint8_t sa;
};

// I-format macro
#define I_OP(I) R_OP(I)
#define I_RS(I) R_RS(I)
#define I_RT(I) R_RT(I)
#define I_CONST(I) (I & CONST_BITS)
#define I_TYPE(I) (!R_TYPE(I) && !J_TYPE(I))

struct I_field {
    char op_name[8];
    uint8_t rt;
    uint8_t rs;

    uint8_t opcode;
    uint8_t flag;

    int (* funcptr)(int, int, int);
    uint16_t immediate;
};

// J-format macro
#define J_OP(I) R_OP(I)
#define J_ADDR(I) I & 0x3FFFFFF
#define J_TYPE(I) (0b00001 == (R_OP(I) >> 1))

struct J_field {
    char op_name[8];
    uint32_t addr;

    uint8_t opcode;
    uint8_t flag;

    int (* funcptr)(int);
};

// Coprocessor Instructions
#define EXTRA_OP(I) R_OP(I)
#define EXTRA_FORMAT(I) R_RS(I)
#define EXTRA_FT(I) R_RT(I)
#define EXTRA_FS(I) R_RD(I)
#define EXTRA_FD(I) R_SA(I)
#define EXTRA_FUNC(I) R_FUNC(I)
#define EXTRA_TYPE(I) (0b0100 == (EXTRA_OP(I) >> 2))

struct Extra_field {
    char op_name[8];
    uint8_t opcode;
    uint8_t format;
    uint8_t funct;

    // FT == RT, FS == RD, FD == SA
    uint8_t flag;

    uint8_t ft;
    uint8_t fs;
    uint8_t fd;
};

#define ANON     0
#define IMMED    0b1
#define LABEL    0b10
#define IMMED_RS 0b100
#define RS       0b1000
#define RT       0b10000
#define SA       0b100000
#define RD       0b1000000
#define REVER    0b10000000  // RD RS RT -> RD RT RS
#define FT       RT
#define FS       RD
#define FD       SA

struct Instruction {
    char type; 
    struct R_field *R_type;
    struct J_field *J_type;
    struct I_field *I_type;
    struct Extra_field *E_type;
    uint32_t data;
};


#define INST_MEM 0xFFFF

struct Instruction code[INST_MEM];

bool __init_instruction(struct program *prog);

int instruction_store(struct program *prog);

bool instruction_terminator(struct Instruction **curr);




#endif /*  _MIPS_INSTRUCTION_H */
