#ifndef _MIPS_UNIT_
#define _MIPS_UNIT_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "unit.h"
#include "instruction.h"

#define DATA_SPACE 0xFFFFF
#define CODE_SPACE 0xFFFF

// CPU
struct _cpu cpu;

// Controler
struct _control controler;

//  Memory
uint8_t data[DATA_SPACE];
struct instruction code[CODE_SPACE]; // instruction database

// Pipeline Register
struct _IF_ID IF_ID;
struct _ID_EX ID_EX;
struct _EX_MEM EX_MEM;
struct _MEM_WB MEM_WB;

void __init_cpu(struct program *prog)
{
    int  i = 0;
    int data = 0;
    if (!prog->bin_file || !prog->reg_file)
        return;
    
    for (i = 0; i < 32; i++) {
        if (!!prog->reg_file) {
            if (fscanf(prog->reg_file, "%d", &data) != 1) {
                printf("CPU Initialization is fail\n");
                return;
            }
        }

        cpu.reg_file[i] = data;
    }

    cpu.cycle = 0;
    cpu.counter = 0;
}

void __init_code(struct program *prog)
{
    int inst, curr;
    char buff[11];

    curr = 0;
    while (fgets(buff, 11, prog->bin_file) != NULL) {
        inst = (int)strtol(buff, NULL, 16);
        code[curr].data = inst;
        curr++;
    }

    prog->inst_count = curr;
}

bool __init_prog(int argc, char **argv, struct program *prog)
{
    // without register file
    if (argc == 3 || argc == 4) {
        strcpy(prog->fbin_name, argv[1]);        
        prog->bin_file = fopen(prog->fbin_name, "r");

        prog->cycle = atoi(argv[2]);

        // Set mode bit
        controler.mode_bit = atoi(argv[3]);
        
        // Optional
        if (argc == 5) {
            strcpy(prog->freg_name, argv[4]);
            prog->reg_file = fopen(prog->freg_name, "r");
        }
        
        __init_code(prog);
        __init_cpu(prog);
        __init_control(prog);
        return true;
    }

    // Error
    return false;
}

bool __exit_prog(struct program *prog)
{
    if (!!prog->bin_file)
        fclose(prog->bin_file);
    
    if (!!prog->reg_file)
        fclose(prog->reg_file);
    
    return true;
}

void __init_control(struct program *prog)
{
    controler.RegDst = false;
    controler.RegWrite = false;
    controler.ALUSrc = false;
    controler.MemRead = false;
    controler.MemWrite = false;
    controler.MemtoReg = false;

    controler.branch = false;
    controler.jump = false;
    controler.ALUOp = false;
    controler.Unsign = false;
    
    controler.MemSize = 0;
    controler.MemSign = true;
    
    controler.PCSrc = false;
    controler.stall = false;
    controler.load_store = false;

    controler.MemStatus.status = 0;
    controler.MemStatus.size = 0;
    controler.MemStatus.addr = 0;
    controler.MemStatus.data = 0;
}

void IF_ID_set(struct instruction *inst)
{
    assert(inst != NULL);

    IF_ID.counter = inst->counter;

    IF_ID.inst = (struct instruction *)inst;
}

void flush_IF_ID()
{
    IF_ID.counter = 0;

    IF_ID.inst = NULL;
}

void ID_EX_set(struct instruction *inst)
{
    assert(inst != NULL);
    // ID stage result
    ID_EX.counter = IF_ID.counter;

    ID_EX.read_data1 = inst->read_data1;
    ID_EX.read_data2 = inst->read_data2;

    ID_EX.sign_extended = inst->sign_extended;

    ID_EX.dest_reg0 = R_RT(inst->data);
    ID_EX.dest_reg1 = R_RD(inst->data);

    // Control unit
    ID_EX.Unsign = controler.Unsign;

    ID_EX.ALUSrc = controler.ALUSrc;
    ID_EX.ALUOp = controler.ALUOp;
    ID_EX.RegDst = controler.RegDst;

    ID_EX.MemWrite = controler.MemWrite;
    ID_EX.MemRead = controler.MemRead;
    ID_EX.MemSign = controler.MemSign;
    ID_EX.MemSize = controler.MemSize;
    
    ID_EX.branch = controler.branch;
    ID_EX.jump = controler.jump;

    ID_EX.RegWrite = controler.RegWrite;
    ID_EX.MemtoReg = controler.MemtoReg;
    ID_EX.inst = IF_ID.inst;
}

void flush_ID_EX()
{
    ID_EX.counter = 0;

    ID_EX.read_data1 = 0;
    ID_EX.read_data2 = 0;

    ID_EX.sign_extended = 0;

    ID_EX.dest_reg0 = 0;
    ID_EX.dest_reg1 = 0;

    // Control unit
    ID_EX.Unsign = false;

    ID_EX.ALUSrc = false;
    ID_EX.ALUOp = false;
    ID_EX.RegDst = false;

    ID_EX.MemWrite = false;
    ID_EX.MemRead = false;
    ID_EX.MemSign = false;
    ID_EX.MemSize = false;
    
    ID_EX.branch = false;
    ID_EX.jump = false;

    ID_EX.RegWrite = false;
    ID_EX.MemtoReg = false;
    ID_EX.addr_result = 0;

    ID_EX.inst = NULL;
}

void EX_MEM_set(struct instruction *inst)
{
    assert(inst != NULL);
    // EX stage result
    EX_MEM.counter = ID_EX.counter;
    EX_MEM.addr_result = inst->addr_result;
    
    EX_MEM.zero = inst->zero;
    EX_MEM.ALU_result = inst->ALU_result;

    EX_MEM.write_mem_data = inst->write_mem_data;
    EX_MEM.dest_reg = inst->dest_reg;

    // Control unit
    EX_MEM.Unsign = ID_EX.Unsign;

    EX_MEM.MemWrite = ID_EX.MemWrite;
    EX_MEM.MemRead = ID_EX.MemRead;
    EX_MEM.MemSign = ID_EX.MemSign;
    EX_MEM.MemSize = ID_EX.MemSize;
    
    EX_MEM.branch = ID_EX.branch;
    EX_MEM.jump = ID_EX.jump;

    EX_MEM.RegWrite = ID_EX.RegWrite;
    EX_MEM.MemtoReg = ID_EX.MemtoReg;

    EX_MEM.inst = ID_EX.inst;
}

void flush_EX_MEM()
{
    EX_MEM.counter = 0;
    EX_MEM.addr_result = 0;
    
    EX_MEM.zero = false;
    EX_MEM.ALU_result = 0;

    EX_MEM.write_mem_data = 0;
    EX_MEM.dest_reg = 0;

    // Control unit
    EX_MEM.Unsign = false;

    EX_MEM.MemWrite = false;
    EX_MEM.MemRead = false;
    EX_MEM.MemSign = false;
    EX_MEM.MemSize = 0;
    
    EX_MEM.branch = false;
    EX_MEM.jump = false;

    EX_MEM.RegWrite = false;
    EX_MEM.MemtoReg = false;

    EX_MEM.inst = NULL;

}

void MEM_WB_set(struct instruction *inst)
{
    MEM_WB.read_mem_data = inst->read_mem_data;
    MEM_WB.dest_reg = inst->dest_reg;
    MEM_WB.ALU_result = inst->ALU_result;
    
    MEM_WB.addr_result = inst->addr_result;

    MEM_WB.Unsign = EX_MEM.Unsign;

    MEM_WB.RegWrite = EX_MEM.RegWrite;
    MEM_WB.MemtoReg = EX_MEM.MemtoReg;
    MEM_WB.branch = EX_MEM.branch;
    MEM_WB.counter = EX_MEM.counter;

    MEM_WB.inst = EX_MEM.inst;
}

void flush_MEM_WB()
{
    MEM_WB.read_mem_data = 0;
    MEM_WB.dest_reg = 0;
    MEM_WB.ALU_result = 0;
    
    MEM_WB.addr_result = 0;

    MEM_WB.Unsign = false;

    MEM_WB.RegWrite = false;
    MEM_WB.MemtoReg = false;
    MEM_WB.branch = false;
    MEM_WB.counter = 0;

    MEM_WB.inst = NULL;
}

/***********************[ Units ]***********************/

// IF
// inst_memory()
// : Store instruction data and meta data for control
bool inst_memory(struct instruction *inst)
{
    uint32_t curr = inst->data;

    if (R_TYPE(curr))
    {
        struct R_field *meta = inst_r_find(curr);

        inst->type = meta->type;
        inst->flag = meta->flag;
    }
    else if (I_TYPE(curr))
    {
        struct I_field *meta = inst_i_find(curr);

        inst->type = meta->type;
        inst->flag = meta->flag;
    }
    else if (J_TYPE(curr))
    {
        struct J_field *meta = inst_j_find(curr);

        inst->type = meta->type;
        inst->flag = meta->flag;
    }

    if (inst->type == 0) return false;

    return true;
}


// ID
bool control(struct instruction *inst)
{
    // Important: Only control unit to be used can be set control variable
    // If the condtion is set, is fine since pipelined registers in next time
    uint8_t type = inst->type;
    uint8_t op = 0;

    if (type & _D) {
        if (!(type & _I)) {
            msg("control: Data transfer is not I-Type\n");
            return false;
        }

        op = I_OP(inst->data);
        
        controler.ALUOp = 0b00;
        controler.ALUSrc = true;
        controler.RegDst = false;
        
        if (op == 0b001111) {
            // lui, load sign_extended immediate value into regster(RT)
            controler.RegWrite = true;

            // Not used (For initalization)
            controler.MemRead = false;
            controler.MemWrite = false;
            controler.MemtoReg = false;

            controler.branch = false;
            controler.jump = false;
            controler.Unsign = false;

            controler.MemSize = 0;
            controler.MemSign = false;

            return true;
        }

        // Store
        if (op == 0b101011 || op == 0b101001 || op == 0b101000) { // sw, sh, sb
            controler.RegWrite = false;

            controler.MemRead = false;
            controler.MemWrite = true;
            controler.MemtoReg = false;
        }
        // Load
        else {
            controler.RegWrite = true;

            controler.MemRead = true;
            controler.MemWrite = false;
            controler.MemtoReg = true;
        }
        
        // Set memory size to read
        if (op == 0b100011 || op == 0b101011) // lw, sw
            controler.MemSize = 4;
        else if (op == 0b100001 || op == 0b100101 || op == 0b101001) // lh, lhu, sh
            controler.MemSize = 2;
        else if (op == 0b100000 || op == 0b100100 || op == 0b101000) // lb, lbu, sb
            controler.MemSize = 1;
        
        // Unsigned instruction
        if (op == 0b100101 || op == 0b100100) // lhu, lbu
            controler.MemSign = true;
        else
            controler.MemSign = false;
        
        // Not used (For initalization)
        // offset is all unsigned
        controler.Unsign = false;

        controler.branch = false;
        controler.jump = false;
    }
    else if (type & _B && !(type & _R)) {
        // Branch is not taken ==> At MEM stage, flush if the condition is true
        if (type & _I && inst->flag & IMMED) { // slti, sltiu
            op = I_OP(inst->data);

            controler.ALUOp = 0b10;
            
            controler.RegWrite = true;
            controler.ALUSrc = true;

            controler.branch = false;

            if (op == 0b001011)
                controler.Unsign = true;
            else
                controler.Unsign = false;
        }
        else { // beq, bne
            controler.ALUOp = 0b01;
            
            controler.RegWrite = false;
            controler.ALUSrc = false;

            controler.branch = true;

            controler.Unsign = false;
        }

        // Common
        controler.RegDst = false;

        // Not used
        controler.MemRead = false;
        controler.MemWrite = false;
        controler.MemtoReg = false;

        controler.jump = false;

        controler.MemSize = 0;
        controler.MemSign = false;
    }
    else if (type & _A || type & _L) {
        controler.ALUOp = 0b10;

        if (type & _R) {
            controler.RegDst = true;
            controler.ALUSrc = false;
        }
        else if (type & _I) {
            controler.RegDst = false;
            controler.ALUSrc = true;
        }
        
        // sign or not
        // R-type : logical instructions
        // I-type : lhu, lbu, lui
        if (type & _L) {
            controler.Unsign = true;
        }
        else {
            controler.Unsign = false;
        }

        // Common
        controler.RegWrite = true;

        // Not used (For initalization)
        controler.MemRead = false;
        controler.MemWrite = false;
        controler.MemtoReg = false;
        
        controler.branch = false;
        controler.jump = false;
        controler.MemSize = 0;
        controler.MemSign = false;
    }
    else if (type & _R && type & _B) { // slt, sltu
        uint8_t funct = R_FUNC(inst->data);
        controler.ALUOp = 0b10;

        controler.RegDst = true;
        controler.ALUSrc = false;

        if (funct == 0b101011)
            controler.Unsign = true;
        else
            controler.Unsign = false;
        
        // Common
        controler.RegWrite = true;

        // Not used (For initalization)
        controler.MemRead = false;
        controler.MemWrite = false;
        controler.MemtoReg = false;
        
        controler.branch = false;
        controler.jump = false;
        controler.MemSize = 0;
        controler.MemSign = false;
    }
    else if (type & _J) {
        // Jump is always taken ==> At ID stage, always jump nothing to do
        op = J_OP(inst->data);
        
        controler.ALUOp = 0b11; // nothing to do

        if (type & _R) { // jr
            controler.RegWrite = true;
        }
        else { // jal, j
            controler.RegWrite = false;
        }

        // Common
        controler.jump = true;
        controler.RegDst = true;

        // Not used (For initalization)
        controler.ALUSrc = false;
        controler.MemRead = false;
        controler.MemWrite = false;
        controler.MemtoReg = false;

        controler.branch = false;
        controler.Unsign = false;

        controler.MemSize = 0;
        controler.MemSign = false;
    }
    else {
        // Error Case, Nothing
        return false;
    }

    return true;
}

bool registers(struct instruction *inst)
{
    // Read registers based on the instruction
    // We only consider how many instructions have to read
    uint8_t type = inst->type;
    uint8_t flag = inst->flag;
    uint8_t reg1 = 0, reg2 = 0;

    if (inst->flag == ANON) return false;

    // Read registers
    if (!(type & _J)) {
        // shmat field (SA flag) : shift amount
        if (flag & SA) {
            reg1 = R_RT(inst->data);
            inst->read_data1 = cpu.reg_file[reg1];
            inst->read_data2 = R_SA(inst->data);
            return true;
        }

        // read 1
        reg1 = R_RS(inst->data);
        inst->read_data1 = cpu.reg_file[reg1];

        // read 2
        if (flag & RT) {
            reg2 = R_RT(inst->data);
            inst->read_data2 = cpu.reg_file[reg2];
        }
    }
    else {
        // Static for 'jr'
        // No reg2
        reg1 = R_RS(inst->data);
        inst->read_data1 = cpu.reg_file[reg1];
    }

    return true;
}

bool sign_extend(struct instruction *inst)
{
    uint16_t immediate = I_CONST(inst->data);
    int sign = 0;

    // Sign extention automtically is done
    if (immediate >> 15) {
        sign = CONST_BITS << 16;
    }

    if (controler.Unsign) {
        sign = 0;
    }

    inst->sign_extended = sign + immediate;

    return true;
}

int jump_handler(struct instruction *inst)
{
    if (ID_EX.jump) {
        uint32_t target = J_ADDR(inst->data);
        uint8_t opcode = J_OP(inst->data);

        msg("\n------------- JUMP CYCLE=%d, Inst=0x%08X -------------\n", cpu.cycle, inst->data);
        if (inst->type & _R) { // jr
            // ID_EX.read_data1 == data of r31, that is forwarded
            ID_EX.addr_result = ID_EX.read_data1;
            msg("When IF, Jump: jr (addr_result == R[31]), Curr PC: %04X ---> %04X, R[31]=%04X\n", ID_EX.counter - 4, ID_EX.addr_result, cpu.reg_file[31]);
            return 0;
        }

        // left shift 2 gate
        target <<= 2;
        ID_EX.addr_result = target;
        
        msg("When IF, Jump: %s, Curr PC: %04X ---> %04X\n", inst_j_find(inst->data)->op_name, ID_EX.counter - 4, ID_EX.addr_result);
        
        // jal, store return address
        if (opcode == 0b000011) {
            cpu.reg_file[31] = ID_EX.counter;
            msg("Jal Stores R[31]=%08X\n", cpu.reg_file[31]);
        }
    }

    return 0;
}

void hazard_detection(struct instruction *inst)
{
    // In IF() Stage after set pipeline register
    
    if (ID_EX.MemRead) {
        uint8_t ID_EX_RT = R_RT((ID_EX.inst)->data);
        uint8_t IF_ID_RS = R_RS((IF_ID.inst)->data);
        uint8_t IF_ID_RT = R_RT((IF_ID.inst)->data);

        // Load-Use / Load-Store case can occur
        if ((ID_EX_RT == IF_ID_RS) || 
            (ID_EX_RT == IF_ID_RT)) {
            
            // Convention, Update controler for Load-Store case which is not nessary to stall
            control(IF_ID.inst);
            if (controler.MemWrite && ID_EX.MemRead) {
                // IF Stage Inst = Store, ID Stage Inst = Load
                return;
            }
            // Stall pipeline
            // How to stall...?
            // In ID() stage after EX() stage, ID_EX.inst == EX_MEM == inst
            // Current register --> buble
            flush_IF_ID();
            controler.stall = true;
        }
    }
}


// EX
int ALU(struct instruction *inst)
{
    uint8_t action = ALU_control(inst);
    int ret = 0;

    switch (action) {
        case ALU_AND:
            ret = inst->read_data1 & inst->read_data2;
            break;

        case ALU_OR:
            ret = inst->read_data1 | inst->read_data2;
            break;

        case ALU_ADD:
            ret = inst->read_data1 + inst->read_data2;
            break;

        case ALU_SUB:
            ret = inst->read_data1 - inst->read_data2;
            break;

        case ALU_SLT:
            if (ID_EX.Unsign)
                ret = ((unsigned int)inst->read_data1) < ((unsigned int)inst->read_data2);
            else
                ret = (inst->read_data1) < (inst->read_data2);
            break;

        case ALU_NOR:
            ret = (inst->read_data1 | inst->read_data2) ^ 0xFFFFFFFF;
            break;

        case ALU_SLL:
            ret = ((unsigned int)inst->read_data1) << ((unsigned int)inst->read_data2);
            break;

        case ALU_SRL:
            ret = ((unsigned int)inst->read_data1) >> ((unsigned int)inst->read_data2);
            break;

        case ALU_LUI:
            ret = inst->read_data2 << 16;
            break;
    }

    // branch instruction needs
    // Check the branch can be taken
    inst->zero = false;
    if (inst->type & _B && inst->type & _I) {
        uint8_t op = I_OP(inst->data);
        if ((op == 0b000100 && ret == 0) ||
            (op == 0b000101 && ret != 0)) { // beq, bne
            inst->zero = true;
        }
        msg("EX::Branch is EX Stage Zero=%d\n", inst->zero);
    }
    
    return ret;
}

int ALU_control(struct instruction *inst)
{
    // ALU has 6 sets
    // ALU action is done by ALUOp and function
    /*
        ALUOp
        00 (add) : load or store
        01 (sub) : beq
        10 (func) : artith/logical operation
    */
    uint8_t action = 0;
    uint8_t op = ID_EX.ALUOp;

    // load or store instructions
    switch (op) {
        // data transfer type
        case 0b00:
            if (I_OP(inst->data) == 0b001111) // lui
                action = ALU_LUI;
            else
                action = ALU_ADD;
            break;
        
        // branch
        case 0b01:
            action = ALU_SUB;
            break;
        
        // alithmatic/logical operation
        case 0b10:
            if (inst->type & _R) {
                uint8_t funct = R_FUNC(inst->data);

                if (funct == 0b100000)
                    action = ALU_ADD;
                else if (funct == 0b100010)
                    action = ALU_SUB;
                else if (funct == 0b100101)
                    action = ALU_OR;
                else if (funct == 0b100111)
                    action = ALU_NOR;
                else if (funct == 0b000000)
                    action = ALU_SLL;
                else if (funct == 0b000010)
                    action = ALU_SRL;
                else if (funct == 0b101010 || funct == 0b101011)
                    action = ALU_SLT;
            }
            else if (inst->type & _I) {
                uint8_t opcode = I_OP(inst->data);

                if (opcode == 0b001000) // addi
                    action = ALU_ADD;
                else if (opcode == 0b001000) // andi
                    action = ALU_AND;
                else if (opcode == 0b001101) // ori
                    action = ALU_OR;
                else if (opcode == 0b001010 || opcode == 0b001011) // slti, sltiu
                    action = ALU_SLT;
            }
            break;
        
        // optional
        default:
            break;
    }

    return action;
}

int address_calculator(struct instruction *inst)
{
    // shft left 2 and addr results
    inst->addr_result = (ID_EX.counter) + (ID_EX.sign_extended << 2);
    return 0;
}


// MEM
int data_memory(struct instruction *inst) {
    // input: address, write data
    uint32_t addr = 0;
    int read_data = 0;
    uint8_t i = 0;

    addr = inst->ALU_result & DATA_SPACE;

    if (EX_MEM.MemRead) {
        msg("\n------------- Memory IO CYCLE=%d, Inst=0x%08X -------------\n", cpu.cycle, inst->data);

        // control unit will set MemSize depens on instruction types
        memcpy(&read_data, &data[addr], EX_MEM.MemSize);
        
        if (EX_MEM.MemSign) {
            inst->read_mem_data = (unsigned)read_data;
        }
        else {
            uint32_t sign = 0;
            if (read_data >> (EX_MEM.MemSize * 8 - 1)) {
                if (EX_MEM.MemSize == 1) sign = 0xFFFFFFF0;
                else if (EX_MEM.MemSize == 2) sign = 0xFFFF0000;
            }

            inst->read_mem_data = sign | read_data;
        }
        
        controler.MemStatus.status = 1;
        controler.MemStatus.addr = addr;
        controler.MemStatus.data = inst->read_mem_data;
        controler.MemStatus.size = EX_MEM.MemSize;

        msg("data_memory: read data, size=%d, read_mem_data=%d\n", EX_MEM.MemSize, inst->read_mem_data);
        for (i = 0; i < EX_MEM.MemSize; i++) {
            msg("0x%08X | %02X |\n", addr + i, data[addr + i]);
        }
        msg("\n");
    }
    else if (EX_MEM.MemWrite) {
        msg("\n------------- Memory IO CYCLE=%d, Inst=0x%08X -------------\n", cpu.cycle, inst->data);

        memcpy(&data[addr], &inst->write_mem_data, EX_MEM.MemSize);

        controler.MemStatus.status = 2;
        controler.MemStatus.addr = addr;
        controler.MemStatus.data = inst->write_mem_data;
        controler.MemStatus.size = EX_MEM.MemSize;
        
        msg("data_memory: write data, size=%d, wirte_mem_data=%08X\n", EX_MEM.MemSize, inst->write_mem_data);
        for (i = 0; i < EX_MEM.MemSize+12; i++) {
            msg("0x%08X | %02X |\n", addr + i, data[addr + i]); 
        }
        msg("\n");
    }
    else {
        controler.MemStatus.status = 0;
    }
    
    // Another exceptions will be dealt with WB state by referecing the result of ALU
    return 0;
}

int big_to_little(int data, int size)
{
    uint8_t bytes[4] = {0, };
    int ret = 0;
    int i = 0;

    for (i = 0; i < size/2; i++)
        bytes[i] = (uint8_t)((data >> (i * 8)) & 0xFF);
    
    for (i = 0; i < size/2; i++)
        ret |= ((int)bytes[i] << (i * 8));
    
    return ret;
}

int little_to_big(int data, int size)
{
    uint8_t bytes[4] = {0, };
    int ret = 0;
    int i = 0;

    for (i = 0; i < size/2; i++)
        bytes[i] = (uint8_t)((data >> (i * 8)) & 0xFF);
    
    for (i = 0; i < size/2; i++)
        ret |= ((int)bytes[i] << ( (size)));

    return 0;
}

void branch_handler(struct instruction *inst)
{
    // If branch is occured, we need to consider pipeline registers
    // we need to flash IF_ID, ID_EX, EX_MEM
    // flush function remove instruction pointer --> EX(), ID() is passed automatically
    flush_IF_ID();
    flush_ID_EX();
    flush_EX_MEM();
    
    msg("\n------------- Branch CYCLE=%d, Inst=0x%08X -------------\n", cpu.cycle, inst->data);
    msg("Inst: %s\n", inst_i_find(inst->data)->op_name);
    msg("When IF, Curr PC: %04X ---> %04X\n", MEM_WB.counter - 4, MEM_WB.addr_result);
}


// WB

// Data forwarding
void data_forwarding(struct instruction *inst)
{
    // Check EX/MEM State and MEM/WB State
    // And, Figure out data forwarding is ouccured
    // This reture values used for two MUX before ALU
    // Current instruction is after ID stage
    uint8_t ID_EX_flag = inst->flag;
    uint8_t ID_EX_type = inst->type;
    uint8_t reg1 = 0, reg2 = 0;

    struct instruction *EX_MEM_inst = EX_MEM.inst, *MEM_WB_inst = MEM_WB.inst;
    uint8_t EX_MEM_reg = 0, MEM_WB_reg = 0;
    bool isRecent = false;

    if (!(ID_EX_flag & RS || ID_EX_flag & RT || ID_EX_flag & SA)) return;


    // Case: read_data1
    isRecent = false;
    if (ID_EX_flag & RS) {
        reg1 = R_RS(inst->data); // RS register location is same both R and I
        // Case 01
        // Source: EX/MEM
        if (EX_MEM_inst != NULL) {
            if (EX_MEM.RegWrite) {
                // Get desination register number
                if (EX_MEM_inst->type & _R)
                    EX_MEM_reg = R_RD(EX_MEM_inst->data);
                else if (EX_MEM_inst->type & _I)
                    EX_MEM_reg = I_RT(EX_MEM_inst->data);
                
                if (reg1 == EX_MEM_reg && EX_MEM_reg != 0) {
                    inst->read_data1 = EX_MEM.ALU_result;
                    ID_EX.read_data1 = EX_MEM.ALU_result;
                    isRecent = true;

                    msg("\nEX::DataFowarding at %08X, Cycle=%d\n", inst->data, cpu.cycle);
                    msg("Forwarding EX_MEM=%08X | 0x%08X --> read_data1\n", EX_MEM_inst->data, ID_EX.read_data1);
                }
            }
        }

        // Case 10
        // Source: MEM/WB
        if (MEM_WB_inst != NULL) {
            if (MEM_WB.RegWrite) {
                // Get desination register number
                if (MEM_WB_inst->type & _R)
                    MEM_WB_reg = R_RD(MEM_WB_inst->data);
                else if (MEM_WB_inst->type & _I)
                    MEM_WB_reg = I_RT(MEM_WB_inst->data);
                
                // NEED handle double data forwarding
                if (reg1 == MEM_WB_reg && MEM_WB_reg != 0 && !isRecent) {
                    if (MEM_WB.MemtoReg) {
                        inst->read_data1 = MEM_WB.read_mem_data;
                        ID_EX.read_data1 = MEM_WB.read_mem_data;
                    }
                    else {
                        inst->read_data1 = MEM_WB.ALU_result;
                        ID_EX.read_data1 = MEM_WB.ALU_result;
                    }
                    msg("\nEX::DataFowarding at %08X, Cycle=%d\n", inst->data, cpu.cycle);
                    msg("Forwarding MEM_WB=%08X | 0x%08X --> read_data1\n", MEM_WB_inst->data, ID_EX.read_data1);
                }
            }
        }

        // Case 00 if nothing to do
    }


    // Case: read_data2 if only R-type register
    isRecent = false;
    if ( (ID_EX_flag & RT) && 
         ( (ID_EX_type & _R) || (ID_EX_type & _D) || 
           ((ID_EX_type & _B) && (ID_EX_flag & LABEL)) ) ) {
        
        if (ID_EX_flag & RT)
            reg2 = R_RT(inst->data);
        
        // Case 01
        // Source: EX/MEM
        if (EX_MEM_inst != NULL) {
            if (EX_MEM.RegWrite) {
                // Get desination register number
                if (EX_MEM_inst->type & _R)
                    EX_MEM_reg = R_RD(EX_MEM_inst->data);
                else if (EX_MEM_inst->type & _I)
                    EX_MEM_reg = I_RT(EX_MEM_inst->data);
                
                if (reg2 == EX_MEM_reg && EX_MEM_reg != 0) {
                    inst->read_data2 = EX_MEM.ALU_result;
                    ID_EX.read_data2 = EX_MEM.ALU_result;

                    isRecent = true;

                    // Check Load-Store Case, it will do dataforwarding again
                    if (ID_EX.MemWrite && EX_MEM.MemRead) {
                        controler.load_store = true;
                    }

                    msg("\nEX::DataFowarding at %08X, Cycle=%d\n", inst->data, cpu.cycle);
                    msg("Forwarding EX_MEM=%08X | 0x%08X --> read_data2\n", EX_MEM_inst->data, ID_EX.read_data2);
                }
            }
        }

        // Case 10
        // Source: MEM/WB
        if (MEM_WB_inst != NULL) {
            if (MEM_WB.RegWrite) {
                // Get desination register number
                if (MEM_WB_inst->type & _R)
                    MEM_WB_reg = R_RD(MEM_WB_inst->data);
                else if (MEM_WB_inst->type & _I)
                    MEM_WB_reg = I_RT(MEM_WB_inst->data);
                
                // NEED handle double data forwarding
                if (reg2 == MEM_WB_reg && MEM_WB_reg != 0 && !isRecent) {
                    inst->read_data2 = MEM_WB.ALU_result;
                    ID_EX.read_data2 = MEM_WB.ALU_result;

                    msg("\nEX::DataFowarding at %08X, Cycle=%d\n", inst->data, cpu.cycle);
                    msg("Forwarding MEM_WB=%08X | 0x%08X --> read_data2\n", MEM_WB_inst->data, ID_EX.read_data2);
                }
            }
        }

        // Case 00 if nothing to do
    }
    return;
}

void data_forwarding_load_store(struct instruction *inst)
{
    // load-store case
    if (controler.load_store) {
        inst->write_mem_data = MEM_WB.read_mem_data;
        controler.load_store = false;
        msg("load_store\n");
    }
}

// Debugging
void track(struct instruction *inst)
{
    uint8_t flag = inst->flag;
    uint8_t type = inst->type;
    bool isID;

#ifdef DO_DEBUG
    uint32_t data = inst->data;
    uint8_t reg = 0;
#endif

    if (inst->data != TARGET) return;

    msg("\n------------- TRACKER CYCLE=%d -------------\n", cpu.cycle);
    msg("Target Instruction == %08X\n", inst->data);

    // Current working stage
    if (MEM_WB.inst == inst && EX_MEM.inst != inst) msg("Stage: WB\n");
    else if (MEM_WB.inst == inst) msg("Stage: MEM\n");
    else if (EX_MEM.inst == inst) msg("Stage: EX\n");
    else if (ID_EX.inst == inst) {
        msg("Stage: ID\n");
        isID = true;
    }
    else if (IF_ID.inst == inst) msg("Stage: IF\n");
    else msg("Stage: WB\n");

    // Print want to check
    msg("Type: ");
    if (type & _I) msg("I ");
    if (type & _A) msg("Arith ");
    if (type & _R) msg("R ");
    if (type & _J) msg("J ");
    if (type & _B) msg("B ");
    if (type & _D) msg("D ");
    if (type & _L) msg("L ");
    if (type == 0) msg("ANON ");
    msg("\n");
    
    // CPU info
    msg("PC=%04X\n", cpu.counter);

    // Target Register
    if (type & _I) {
        if (type & _R && flag & LABEL) {
            msg("RT <- RS op Immediate\n");
            msg("RT R[%d]=%08X\n", I_RT(data), inst->ALU_result);
            
            msg("RS R[%d]=%08X\n", I_RS(data), inst->read_data1);
            msg("Immedi=%08X\n", inst->read_data2);
        }
        else {
            msg("RT ? RS --> Branch!\n");
            msg("RT R[%d]=%08X\n", I_RT(data), inst->read_data2);

            msg("RS R[%d]=%08X\n", I_RS(data), inst->read_data1);
            msg("Immedi=%08X\n", inst->read_data2);
        }
        msg("zero flag=%d\n", inst->zero);

        msg("\nInst: %s\n", inst_i_find(inst->data)->op_name);
        msg("read_data1=%08X\n", inst->read_data1);
        msg("read_data2=%08X\n", inst->read_data2);
        msg("ALU_result=%08X\n", inst->ALU_result);
    }
    else if (type & _R) {
        msg("RD <- ");
        if (type & SA) msg("RT");
        else msg("RS");
        msg(" op ");
        if (type & SA) msg("SA");
        else msg("RT");
        msg("\n");

        msg("RD R[%d]=%08X\n", R_RD(data), cpu.reg_file[reg]);
        if (type & SA) {
            msg("RT R[%d]=%08X\n", R_RT(data), inst->read_data1);
            msg("SA R[%d]=%08X\n", R_SA(data), inst->read_data2);
        }
        else {
            msg("RS R[%d]=%08X\n", R_RS(data), inst->read_data1);
            msg("RT R[%d]=%08X\n", R_RT(data), inst->read_data2);
        }
        msg("\nInst: %s\n", inst_r_find(inst->data)->op_name);
        
        msg("read_data1=%08X\n", inst->read_data1);
        msg("read_data2=%08X\n", inst->read_data2);
        msg("ALU_result=%08X\n", inst->ALU_result);

        // Control Units
        msg(" ID] RegWrite=%d\n", ID_EX.RegWrite);
        msg(" EX] ALUSrc=%d\n", ID_EX.ALUSrc);
        msg(" EX] ALUOp=%d\n", ID_EX.ALUOp);
        msg(" EX] RegDst=%d\n", ID_EX.RegDst);
        msg("MEM] MemWrite=%d\n", ID_EX.MemtoReg);
        msg("MEM] MemRead=%d\n", ID_EX.MemRead);
        msg(" WB] MemtoReg=%d\n", ID_EX.MemtoReg);
        msg("Special]\n");
        msg("\tbranch=%d\n", ID_EX.branch);
        msg("\tjump=%d\n", ID_EX.jump);
    }
    else if (type & _J) {
        msg("\nInst: %s\n", inst_j_find(inst->data)->op_name);
        msg("addr=%04X\n", inst->addr_result);
    }

    msg("ALU_result=%08X\n", inst->ALU_result);
    
    // Control Units`
    if (isID) {
        msg("ALUOp=%08X\n", ID_EX.ALUOp);
    }

    msg("\t\t\t\t (END)\n");
    msg("\n");
}
#endif /*  _MIPS_UNIT_ */
