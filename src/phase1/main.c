#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#include "Instruction.h"
#include "program.h"
#include "unit.h"

/*
    [Project: MIPS SIMULATOR]

    $ ./MIPS_Simulator   {Program_binary}   {Number_of_cycles}   [Initial_register_state]

    Input: 
        MIPS program in binary code
        Number of cycles to run
        Initial register state (optional)
    
    Output: 
        Print the state of the processor after each cycle. Process state includes the following information.
        PC
        Instruction
        Register contents
        Memory I/O
*/

/*
 *  Functions
 */
// Initialization
void __init_mips(struct program *prog);

int mips_fetch(struct program *prog);
void mips_program_counter();
void mips_print(struct program *prog);

//  Helper Routines
bool parser(struct program *prog, int argc, char **argv);



int main(int argc, char **argv)
{
    // Initialize Program Status
    struct program prog;

    if (!parser(&prog, argc, argv)) {
        printf("MIPS: Error invalid arguments\n");
        return 0;
    }

    prog.bin_file = fopen(prog.bin_name, "r");
    if (!prog.bin_file) {
        printf("MIPS: File open error\n");
        return 0;
    }

    //Open register file
    //prog.reg_file = fopen(prog.reg_name, "r");
    //if (!prog.reg_file) {
    //    printf("MIPS: File open error\n");
    //    return 0;
    //}

    __init_mips(&prog);
    while(prog.cycle > cpu.cycle) {
        cpu.cycle++;
        mips_print(&prog);
        mips_fetch(&prog);
        mips_program_counter();
    }

    fclose(prog.bin_file);
    // fclose(prog.reg_file);

    prog.bin_file = NULL;
    prog.reg_file = NULL;
    return 0;
}

//
//  Main Program
//
void __init_mips(struct program *prog)
{
    if (prog == NULL)
        return;
     
    // Read register values from fp
    __init_cpu(prog->reg_file);
    __init_instruction(prog);
}

int mips_fetch(struct program *prog)
{
    int curr = cpu.counter/4;
    int ret = 0;
    int s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    uint8_t flag = 0;
    struct R_field *r_inst;
    struct I_field *i_inst;
    struct J_field *j_inst;
    struct E_field *e_inst;

    // 1. Get value of registers are determined by flags.
    // 2. Execute the function of current instructions.
    // 3. Terminate it.

    switch (code[curr].type)
    {
    case 'R':
        r_inst = code[curr].R_type;
        flag = r_inst->flag;

        // rd, rs, rt
        if (flag == (RD|RS|RT)) {
        }
        // rd, rt, rs
        else if (flag == (RD|RS|RT|REVER)) {
        }
        // rd, rs, sa
        else if (flag == (RD|RS|SA)) {
        }
        // rd, rs
        else if (flag == (RD|RS)) {
        }
        // rs, rt
        else if (flag == (RS|RT)) {
        }
        else if (flag == RS) {
            if (r_inst->funct == 0b001000) {
                s0 = r_inst->rs;
                ret = r_inst->funcptr(0, cpu.registers[s0].data, 0, 0);
            }
        }
        else if (flag == RD) {
        }
        else if (flag == ANON) {
        }
        break;

    case 'I':
        i_inst = code[curr].I_type;
        flag = i_inst->flag;

        if (flag == (RS|RT|LABEL)) {
            if (strcmp("bne", i_inst->op_name) ||
                strcmp("beq", i_inst->op_name)) {
                    s0 = cpu.registers[i_inst->rs].data;
                    s1 = cpu.registers[i_inst->rt].data;

                    ret = i_inst->funcptr(s0, s1, i_inst->immediate);
            }
        }
        else if (flag == (RT|RS|IMMED)) {
        }

        break;
    
    case 'J':
        j_inst = code[curr].J_type;
        flag = j_inst->flag;

        if (flag == LABEL) {
            ret = j_inst->funcptr(j_inst->addr);
        }
        break;
    
    case 'E':
        break;
    }

    return ret;
}

void mips_program_counter()
{
    cpu.counter += 4;
}


void mips_print(struct program *prog)
{
    int curr = cpu.counter/4;

    if (code[curr].data == 0) return;

    if (cpu.cycle != 1) printf("\n");
    printf("Cycle %d\n", cpu.cycle);
    printf("PC: %04X\n", cpu.counter);
    printf("Instruction: %08x\n", code[curr].data);

}

//
//  Helper Functions
//
bool parser(struct program *prog, int argc, char **argv)
{
    // In Phase 1 we don't have register initial values.
    if (argc != 3)
        return false;
     
    strcpy(prog->bin_name, argv[1]);
    prog->cycle = atoi(argv[2]);
    // strcpy(prog->reg_name, argv[3]);
    return true;
}
