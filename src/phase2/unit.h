#ifndef _MIPS_UNIT_H_
#define _MIPS_UNIT_H_

#include <stdint.h>
#include <stdio.h>

#include "instruction.h"

#ifdef DO_DEBUG
#define msg(fmt, ...)  printf(fmt,##__VA_ARGS__) 
#else
#define msg(fmt, ...)
#endif

#define NOPE 0xFFFFFFFF
#define TARGET 0xAC8D0000

//  Program
struct program {
    unsigned int cycle; // Maximum cylce (must be set before execution)
    unsigned int inst_count;

    char fbin_name[30];
    FILE *bin_file;

    char freg_name[30];
    FILE *reg_file;
};


struct _cpu {
    unsigned int cycle; 
    int counter; // current cylce, actual cycle = cycle * 4
    int reg_file[32];
};

// Helper Struct
struct _MemStatus {
    uint8_t status;
    uint8_t size;
    int addr;
    int data;
};

// Control unit (Pipeline)
// :most parts refer the control unit for proper execution
struct _control {
    bool RegDst;    // RegDst: true = other, false = usage of I[15:11] is Destination Register
    bool RegWrite;  // RegWrite: is it write action?
    bool ALUSrc;    // ALUSrc:   by MUX, when calculating address as immediate or label
    bool MemRead;   // MemRead:  load instructions
    bool MemWrite;  // MemWrite: store instructions
    bool MemtoReg;  // MemtoReg: when getting memory contants to a register
                    //           (by output read_mem_data from data memory)
    bool branch;    // bracnh:   is there branch instruction?
    bool jump;      // jump: is it unconditional branch?
    uint8_t ALUOp;  // ALUOp:    operation specifier (only 2 bits used)
    bool Unsign;    // Unsign:     represent instruction sign

    // Additional Control Flags
    uint8_t MemSize; // MemSize: Size of memory to read/write
    bool MemSign; // MemSize: sign of a content of memory

    // global variables shared all stage
    bool PCSrc; // On IF stage, it used for what address is changed since branch
    struct _MemStatus MemStatus;// For Print memory I/O status, memstatus=0  nothing to do, 1: read, 2: write
    bool mode_bit;
    bool stall; // for load-use and load-store hazard
    bool load_store;
};


struct _IF_ID {
    int counter;

    struct instruction *inst;
};

struct _ID_EX {
    /********** ID stage result *********/
    // Counter value
    int counter;

    // Registers
    int read_data1;
    int read_data2;

    // Sign-extend
    int sign_extended;

    // Instruction[20:16]
    uint8_t dest_reg0;

    // Instruction[15:11]
    uint8_t dest_reg1;


    /* Control Unit */
    // Common
    bool Unsign;

    // EX
    bool ALUSrc;
    uint8_t ALUOp;
    bool RegDst;

    // MEM
    bool MemWrite;
    bool MemRead;
    bool MemSign;
    uint8_t MemSize;
    
    bool branch;
    bool jump;

    // WB
    bool MemtoReg;

    // IF (write back)
    bool RegWrite;

    // Jump (Addressing)
    int addr_result;

    struct instruction *inst;
};

struct _EX_MEM {
    // Address calculator
    int counter; // for PC MUX
    int addr_result;

    // ALU
    bool zero;
    int ALU_result;

    // other
    int write_mem_data;
    int dest_reg;

    /* Control Unit */
    // Common
    bool Unsign;

    // MEM
    bool MemWrite;
    bool MemRead;
    bool MemSign;
    uint8_t MemSize;
    
    bool branch;
    bool jump;

    // WB
    bool MemtoReg;

    // IF (write back)
    bool RegWrite;

    struct instruction *inst;
};

struct _MEM_WB {
    int read_mem_data;
    int dest_reg;
    int ALU_result;

    // Optional for Branch
    int addr_result;

    /* Control Unit */
    // Common
    bool Unsign;

    // WB
    bool MemtoReg;

    // IF (write back)
    bool RegWrite;

    bool branch;
    int counter;

    struct instruction *inst;
};


// Initialization Functions
void __init_cpu(struct program *prog);
void __init_code(struct program *prog);
bool __init_prog(int argc, char **argv, struct program *prog);
bool __exit_prog(struct program *prog);
void __init_control(struct program *prog);

// Stage setup
void IF_ID_set(struct instruction *inst);
void flush_IF_ID();

void ID_EX_set(struct instruction *inst);
void flush_ID_EX();

void EX_MEM_set(struct instruction *inst);
void flush_EX_MEM();

void MEM_WB_set(struct instruction *inst);
void flush_MEM_WB();

/***********************[ Units ]***********************/
// IF
bool inst_memory(struct instruction *inst);
void hazard_detection(struct instruction *inst);

// ID
bool registers(struct instruction *inst);
bool sign_extend(struct instruction *inst);
int jump_handler(struct instruction *inst);

// EX
bool control(struct instruction *inst);
int address_calculator(struct instruction *inst);
void data_forwarding(struct instruction *inst);

#define ALU_AND 0b0000
#define ALU_OR  0b0001
#define ALU_ADD 0b0010
#define ALU_SUB 0b0110
#define ALU_SLT 0b0111
#define ALU_NOR 0b1100
#define ALU_SLL 0b1101
#define ALU_SRL 0b1110
#define ALU_LUI 0b1111

int ALU(struct instruction *inst);
int ALU_control(struct instruction *inst);

// MEM
int data_memory(struct instruction *inst);
int big_to_little(int data, int size);
int little_to_big(int data, int size);

void branch_handler(struct instruction *inst);

// Data forwarding
void data_forwarding_load_store(struct instruction *inst);

// Debuging Function
void track(struct instruction *inst);

#endif /*  _MIPS_UNIT_H */