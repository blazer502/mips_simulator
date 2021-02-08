#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "unit.h"
#include "instruction.h"

#define DATA_SPACE 0xFFFFF
#define CODE_SPACE 0xFFFF

extern struct _cpu cpu;
extern struct _control controler;
extern uint8_t data[];
extern uint32_t stack[]; // for branch instruction
extern struct instruction code[]; // instruction database

extern struct _IF_ID IF_ID;
extern struct _ID_EX ID_EX;
extern struct _EX_MEM EX_MEM;
extern struct _MEM_WB MEM_WB;

// Only one program is possible
struct program prog;

// Note current instruction = code[cpu.counter]
int IF();
int ID();
int EX();
int MEM();
int WB();

// Helper functions
void mips_print();

//
//  Main
//
int main(int argc, char **argv)
{
    __init_prog(argc, argv, &prog);

    // Execute the loaded program.
    for (cpu.cycle = 1; cpu.cycle <= prog.cycle; cpu.cycle++) {
        WB();
        MEM();
        EX();
        ID();
        IF();
    }

    __exit_prog(&prog);
    return 0;
}


//
//  Functions
//
int IF()
{
    // Parts
    // : Instruction memory, Adder for PC
    struct instruction *inst;

    // If is it true, previous instruction becomes nop
    if (controler.stall) {
        cpu.counter -= 4;
        controler.stall = false;
    }

    inst = &(code[cpu.counter / 4]);
    
    // Initialize instruction, read address and make it executable format
    inst_memory(inst);
    
    // Pre-update PC (== pointing next instruction)
    inst->counter = cpu.counter + 4;

    mips_print();

    // MUX, Update Address
    if (controler.PCSrc) {
        cpu.counter = MEM_WB.addr_result & CODE_SPACE;
        controler.PCSrc = false;

        msg("\n------ Branch is done PC: %04X, Inst: %08X ------\n", cpu.counter, (MEM_WB.inst)->data);
        // Skip this IF stage for bubles (branch bubles: IF, ID, EX)
        return 0;
    }
    else if (ID_EX.jump) {
        // If previous IF() is execute jump instrion.
        cpu.counter = ID_EX.addr_result & CODE_SPACE;  // why +4? since my implementation of counter checker
        msg("%X\n", cpu.counter);
        msg("\n------ Jump is done PC: %04X, Inst: %08X -------\n", cpu.counter, code[cpu.counter/4].data);

        // Remove Jump condition information
        flush_IF_ID();
        flush_ID_EX();
        // Skip this IF stage for buble (buble after jump: IF, jump buble: ID)
        return 0;
    }
    else
        cpu.counter += 4;
    
    IF_ID_set(inst);
    
    // [Mode 1]
    // hazerd_detection
    if (controler.mode_bit) {
        hazard_detection(inst);
        // After hazard_detection(), IF_ID is nop instruction
    }

    // track(inst);
    return 1;
}

int ID()
{
    struct instruction *inst = IF_ID.inst;

    if (!inst) {
        flush_ID_EX();
        return 0;
    }

    // Parts
    // : Registers, Signed extend
    control(inst);
    registers(inst);
    sign_extend(inst);

    ID_EX_set(inst);

    // [Mode 1]
    // data_forwarding
    if (controler.mode_bit) { 
        data_forwarding(inst);
    }

    // Jump Instruction
    if (inst->type & _J) {
        jump_handler(inst);
        track(inst);
        return 0;
    }

    track(inst);
    return 0;
}

int EX()
{
    struct instruction *inst = ID_EX.inst;

    if (!inst) {
        flush_EX_MEM();
        return 0;
    }

    // Parts
    // : ALU, ALU_control

    // Address_calculator
    address_calculator(inst);

    // Set Write register value
    inst->write_mem_data = inst->read_data2;

    // MUX, r-format case ALUSrc must be false
    if (ID_EX.ALUSrc)
        inst->read_data2 = inst->sign_extended;
    
    // ALU
    inst->ALU_result = ALU(inst);

    // MUX, set destination data
    if (controler.RegDst)
        inst->dest_reg = ID_EX.dest_reg1;
    else
        inst->dest_reg = ID_EX.dest_reg0;
    
    EX_MEM_set(inst);

    if (controler.mode_bit)
        data_forwarding_load_store(inst);
    
    track(inst);
    return 0;
}

int MEM()
{
    struct instruction *inst = EX_MEM.inst;

    if (!inst) {
        flush_MEM_WB();
        return 0;
    }

    data_memory(inst);

    MEM_WB_set(inst);

    // Brach Gate, Figure out updating PC+4 or changing Branch LABEL
    controler.PCSrc = EX_MEM.zero && EX_MEM.branch;
    if (controler.PCSrc) {
        branch_handler(inst);
    }

    track(inst);
    return 0;
}

int WB()
{
    struct instruction *inst = MEM_WB.inst;
    uint8_t write_reg = 0;

    if (!inst) return 0;

    // Write the result of ALU to register
    // Stroe result of current instruction
    // Important: Always write it before read

    // MUX, Get the destination of a register's number
    if (MEM_WB.RegWrite) {
        write_reg = MEM_WB.dest_reg;
    }

    // Write operation
    if (MEM_WB.MemtoReg) {
        cpu.reg_file[write_reg] = MEM_WB.read_mem_data;
        msg("\nWB::Write back MemtoReg Inst=%08X, Cycle=%04X, line=%d :: R[%d] <--- %08X\n", inst->data, inst->counter - 4, (inst->counter - 4) / 4, write_reg, cpu.reg_file[write_reg]);
    }
    // R0 can't be changed
    else if (write_reg) {
        cpu.reg_file[write_reg] = MEM_WB.ALU_result;
        msg("\nWB: Write back Normal Inst=%08X, Cycle=%04X, line=%d :: R[%d] <--- %08X\n", inst->data, inst->counter - 4, (inst->counter - 4) / 4, write_reg, cpu.reg_file[write_reg]);
    }
    
    track(inst);
    return 0;
}

void mips_print()
{
    // Print an instruction is done
    int curr = cpu.counter / 4;
    struct instruction *MEM_WB_inst = NULL;
    int i;

    if (cpu.cycle != 1) printf("\n");

    printf("Cycle %d\n", cpu.cycle);
    printf("PC: %04X\n", curr * 4);
    printf("Instruction: %08X\n", code[curr].data);

    if (code[curr].type & _R) msg("[Decoded Inst] %s\n", inst_r_find(code[curr].data)->op_name);
    else if (code[curr].type & _I) msg("[Decoded Inst] %s\n", inst_i_find(code[curr].data)->op_name);
    else if (code[curr].type & _J) msg("[Decoded Inst] %s\n", inst_j_find(code[curr].data)->op_name);

    printf("Registers:\n");
    
    for (i = 0; i < 32; i++) {
        printf("[%d] %08X\n", i, cpu.reg_file[i]);
    }

    printf("Memory I/O: ");
    
    MEM_WB_inst = MEM_WB.inst;
    if (MEM_WB_inst != NULL) {
        if(MEM_WB_inst->type & _D && controler.MemStatus.status != 0) {
            
            if (controler.MemStatus.status == 1)
                printf("R ");
            else if (controler.MemStatus.status == 2)
                printf("W ");

            printf("%d ", controler.MemStatus.size);
            printf("%04X ", controler.MemStatus.addr);

            if (controler.MemStatus.size == 1)
                printf("%02X", controler.MemStatus.data & 0xFF);
            else if (controler.MemStatus.size == 2)
                printf("%04X", controler.MemStatus.data & 0xFFFF);
            else if (controler.MemStatus.size == 4)
                printf("%08X", controler.MemStatus.data & 0xFFFFFFFF);

            controler.MemStatus.status = 0;
            controler.MemStatus.size = 0;
            controler.MemStatus.addr = 0;
            controler.MemStatus.data = 0;
        }
    }
    printf("\n");

}
