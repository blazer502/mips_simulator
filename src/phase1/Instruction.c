#ifndef _MIPS_INSTRUCTION_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Instruction.h"
#include "functionality.h"

// R-type instruction table 
static struct R_field R_table[29] = {
    {"add",     0, 0, 0, 0b100000, RD|RS|RT},            {"addu",  0, 0, 0, 0b100001, RD|RS|RT},       {"and",     0, 0, 0, 0b100100, RD|RS|RT},
    {"break",   0, 0, 0, 0b001101, ANON},                {"div",   0, 0, 0, 0b011010, RS|RT},          {"divu",    0, 0, 0, 0b011011, RS|RT},
    {"jalr",    0, 0, 0, 0b001001, RD|RS},               {"jr",    0, 0, 0, 0b001000, RS, jr},         {"mfhi",    0, 0, 0, 0b010000, RD},
    {"mflo",    0, 0, 0, 0b010010, RD},                  {"mthi",  0, 0, 0, 0b010001, RS},             {"mtlo",    0, 0, 0, 0b010011, RS},
    {"mult",    0, 0, 0, 0b010011, RS|RT},               {"multu", 0, 0, 0, 0b011000, RS|RT},          {"nor",     0, 0, 0, 0b100101, RD|RS|RT},
    {"or",      0, 0, 0, 0b100101, RD|RS|RT},            {"sll",   0, 0, 0, 0b000000, RD|RT|SA},       {"sllv",    0, 0, 0, 0b000100, RD|RS|RT|REVER},
    {"slt",     0, 0, 0, 0b101010, RD|RS|RT, slt},       {"sltu",  0, 0, 0, 0b101011, RD|RS|RT, sltu}, {"sra",     0, 0, 0, 0b000011, RD|RT|SA},
    {"srav",    0, 0, 0, 0b000111, RD|RT|RS|REVER},      {"srl",   0, 0, 0, 0b000010, RD|RT|SA},       {"srlv",    0, 0, 0, 0b000110, RD|RT|RS|REVER},
    {"sub",     0, 0, 0, 0b100010, RD|RS|RT},            {"subu",  0, 0, 0, 0b100011, RD|RS|RT},       {"syscall", 0, 0, 0, 0b001100, ANON},
    {"xor",     0, 0, 0, 0b100110, RD|RS|RT}
};

static struct I_field I_table[24] = {
    {"addi",  0, 0, 0b001000, RT|RS|IMMED|REVER},       {"addiu",  0, 0, 0b001001, RT|RS|IMMED|REVER},       {"andi",  0, 0, 0b001100, RT|RS|IMMED|REVER},
    {"beq",   0, 0, 0b000100, RT|RS|LABEL, beq},        {"bgez",   1, 0, 0b000001, RS|LABEL},                {"bgtz",  0, 0, 0b000111, RS|LABEL},
    {"blez",  0, 0, 0b000110, RS|LABEL},                {"bltz",   0, 0, 0b000001, RS|LABEL},                {"bne",   0, 0, 0b000101, RT|RS|LABEL, bne},
    {"lb",    0, 0, 0b100000, RT|RS|IMMED},             {"lbu",    0, 0, 0b100100, RT|RS|IMMED},             {"lh",    0, 0, 0b100001, RT|RS|IMMED},
    {"lhu",   0, 0, 0b100101, RT|RS|IMMED},             {"lui",    0, 0, 0b001111, RT|IMMED},                {"lw",    0, 0, 0b100011, RT|RS|IMMED},
    {"lwcl",  0, 0, 0b110001, RT|RS|IMMED},             {"ori",    0, 0, 0b001101, RT|RS|IMMED|REVER},       {"sb",    0, 0, 0b101000, RT|RS|IMMED},
    {"slti",  0, 0, 0b001010, RT|RS|IMMED|REVER, slti}, {"sltiu",  0, 0, 0b001011, RT|RS|IMMED|REVER, slti}, {"sh",    0, 0, 0b101001, RT|RS|IMMED},
    {"sw",    0, 0, 0b101011, RT|RS|IMMED},             {"swcl",   0, 0, 0b111001, RT|RS|IMMED},             {"xori",  0, 0, 0b001110, RT|RS|IMMED|REVER}
};

static struct J_field J_table[2] = {
    {"j", 0, 0b000010, LABEL, j}, {"jal", 0, 0b000011, LABEL, jal}
};

static struct Extra_field Extra_table[9] = {
    {"add.s",   0b010001, 0b10000, 0b000000, FD|FS|FT}, {"cvt.s.w", 0b010001, 0b10100, 0b100000, FD|FS|FT},
    {"cvt.w.s", 0b010001, 0b10000, 0b100100, FD|FS|FT}, {"div.s",   0b010001, 0b10000, 0b000011, FD|FS|FT},
    {"mfc1",    0b010001, 0b00000, 0b000000, FS|FT},    {"mov.s",   0b010001, 0b10000, 0b000110, FD|FS},
    {"mtc1",    0b010001, 0b00100, 0b000000, FS|FT},    {"mul.s",   0b010001, 0b10000, 0b000010, FD|FS|FT},
    {"sub.s",   0b010001, 0b10000, 0b000001, FD|FS|FT}
};


bool __init_instruction(struct program *prog) {
    // Make OPCODE map of opcode for fast searching.
    // Adding indices of I_table.
    instruction_store(prog);
    return true;
}

// Instruction fecher : Act allocate struct Instruction and store operations.
int instruction_store(struct program *prog) {
    int i = 0, num = 0;
    uint8_t flag = 0;
    char buff[10];
    int bin;

    if (feof(prog->bin_file)) return 0;

    num = 0;
    while (fgets(buff, 10, prog->bin_file) != NULL) {
        bin = (int)strtol(buff, NULL, 16);

        // Make Instruction struct.
        code[num].data = bin;

        // Copy the instruction struct of binary.
        if (R_TYPE(bin)) {
            uint8_t funct;
            struct R_field *curr;

            code[num].type = 'R';
            code[num].R_type = (struct R_field *)malloc(sizeof(struct R_field));

            funct = R_FUNC(bin);
            for (i = 0; i < 29; i++) {
                if (R_table[i].funct == funct) {
                    memcpy(code[num].R_type, &R_table[i], sizeof(struct R_field));
                    break;
                }
            }

            curr = code[num].R_type;
            flag = curr->flag;
            if (flag & RD) curr->rd = R_RD(bin);
            if (flag & RS) curr->rs = R_RS(bin);
            if (flag & RT) curr->rt = R_RT(bin);
            if (flag & SA) curr->sa = R_SA(bin);
        }
        else if (I_TYPE(bin)) {
            uint8_t opcode;
            struct I_field *curr;

            code[num].type = 'I';
            code[num].I_type = (struct I_field *)malloc(sizeof(struct I_field));

            opcode = I_OP(bin);
            for (i = 0; i < 24; i++) {
                if (I_table[i].opcode == opcode) {
                    memcpy(code[num].I_type, &I_table[i], sizeof(struct I_field));
                    break;
                }
            }

            curr = code[num].I_type;
            flag = curr->flag;
            if (flag & RT) curr->rt = I_RT(bin);
            if (flag & RS) curr->rs = I_RS(bin);
            if (flag & IMMED ||
                flag & LABEL)
                curr->immediate = I_CONST(bin);

        }
        else if (J_TYPE(bin)) {
            uint8_t opcode;
            struct J_field *curr;

            code[num].type = 'J';
            code[num].J_type = (struct J_field *)malloc(sizeof(struct J_field));

            opcode = J_OP(bin);
            for (i = 0; i < 2; i++) {
                if (J_table[i].opcode == opcode) {
                    memcpy(code[num].J_type, &J_table[i], sizeof(struct J_field));
                    break;
                }
            }

            curr = code[num].J_type;
            flag = curr->flag;
            if (flag & LABEL) curr->addr = J_ADDR(bin);
        }
        else if (EXTRA_TYPE(bin)) {
            uint8_t opcode, format, funct;
            struct Extra_field *curr;

            code[num].type = 'E';
            code[num].E_type = (struct Extra_field *)malloc(sizeof(struct Extra_field));
            
            opcode = EXTRA_OP(bin);
            format = EXTRA_FORMAT(bin);
            funct = EXTRA_FUNC(bin);

            for (i = 0; i < 2; i++)
            {
                if (Extra_table[i].opcode == opcode &&
                    Extra_table[i].format == format &&
                    Extra_table[i].funct == funct)
                {
                    memcpy(code[num].E_type, &Extra_table[i], sizeof(struct Extra_field));
                    break;
                }
            }

            curr = code[num].E_type;
            flag = curr->flag;
            if (flag & FD) curr->fd = EXTRA_FD(bin);
            if (flag & FS) curr->fs = EXTRA_FS(bin);
            if (flag & FT) curr->ft = EXTRA_FD(bin);
        }
        else {
            printf("Exception!!\n");
        }

        num++;
    }

    prog->inst_count = num;
    // We can get flags to set the values of each instructions.
    // Recording register, immediate, etc. by using flag value!! 
    return num;
}

bool instruction_terminator(struct Instruction **curr)
{
    if (curr == NULL) return false;
    if (*curr == NULL) return false;

    switch ((*curr)->type) {
        case 'R':
            free((*curr)->R_type);
            break;

        case 'I':
            free((*curr)->I_type);
            break;

        case 'J':
            free((*curr)->J_type);
            break;

        case 'E':
            free((*curr)->E_type);
            break;

        default:
            printf("MIPS: Invalid Instruction to terminate\n");
    }

    free(*curr);

    return true;
}


#endif /*  _MIPS_INSTRUCTION_ */
