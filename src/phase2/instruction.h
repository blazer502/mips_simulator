#ifndef _MIPS_INSTRUCTION_H_
#define _MIPS_INSTRUCTION_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define REG_BITS 0x1F
#define ARITH_BITS 0x3F
#define CONST_BITS 0xFFFF

#define ANON     0
#define IMMED    0b1
#define LABEL    0b10
#define IMMED_RS 0b100
#define RS       0b1000
#define RT       0b10000
#define SA       0b100000
#define RD       0b1000000
#define REVER    0b10000000  // not represented flag, just for debugging
#define FT       RT
#define FS       RD
#define FD       SA

// *| Instruction type classes (= control unit indetifier)
// 1| _R: R-Type
// 2| _I: I-Type
// 3| _J: Jump (unconditional branch)
// 4| _B: branch (conditional branch)
// 5| _D: Data Transfer Type
// 6| _A: Arithmetic
// 7| _L: Logical
#define _R 0b1
#define _I 0b10
#define _J 0b100
#define _B 0b1000
#define _D 0b10000   // 8
#define _A 0b100000  // 16
#define _L 0b1000000 // 32

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
    uint8_t type;

    uint8_t opcode;
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

    uint8_t type;
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

    uint8_t type;
};

// Control unit is included in struct instruction
struct instruction {
    uint8_t type;
    uint32_t data;
    uint8_t flag;

    // IF
    int counter;
    
    // ID
    int sign_extended;
    int read_data1; // ALU input1
    int read_data2; // ALU input2

    // EX
    int ALU_result;
    int addr_result;
    int dest_reg;
    int write_mem_data;
    bool zero;

    // MEM
    int read_mem_data;

    // WB
    int j_data;

};

// Find an inst from dictionaries.
struct R_field *inst_r_find(uint32_t data);
struct I_field *inst_i_find(uint32_t data);
struct J_field *inst_j_find(uint32_t data);

#endif /*  _MIPS_INSTRUCTION_ */