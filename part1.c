#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"

void print_rtype(char *, Instruction);
void print_itype_except_load(char *, Instruction, int);
void print_load(char *, Instruction);
void print_store(char *, Instruction);
void print_branch(char *, Instruction);
void print_custom(char *, Instruction);
void print_lui(Instruction);
void print_jal(Instruction);
void print_ecall(Instruction);
void write_rtype(Instruction);
void write_itype_except_load(Instruction); 
void write_load(Instruction);
void write_store(Instruction);
void write_branch(Instruction);
void write_custom(Instruction);


void decode_instruction(uint32_t instruction_bits) {
    Instruction instruction = parse_instruction(instruction_bits);
    switch(instruction.opcode) {
        case 0x33:
            write_rtype(instruction);
            break;
        case 0x13:
            write_itype_except_load(instruction);
            break;
        case 0x3:
            write_load(instruction);
            break;
        case 0x23:
            write_store(instruction);
            break;
        case 0x63:
            write_branch(instruction);
            break;
        case 0x37:
            print_lui(instruction);
            break;
        case 0x6F:
            print_jal(instruction);
            break;
        case 0x73:
            print_ecall(instruction);
            break;
        case 0x2b:
            write_custom(instruction);
            break;
        default: // undefined opcode
            handle_invalid_instruction(instruction);
            break;
    }
}

void write_rtype(Instruction instruction) {
    switch (instruction.rtype.funct3) {
        case 0x0:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    print_rtype("add", instruction);
                    break;
                case 0x1:
                    print_rtype("mul", instruction);
                    break;
                case 0x20:
                    print_rtype("sub", instruction);
                    break;
                default:
                    handle_invalid_instruction(instruction);
                break;      
            }
            break;
        case 0x1:
            switch (instruction.rtype.funct7) {
                case 0x0:
                print_rtype("sll", instruction);
                break;
                case 0x1:
                print_rtype("mulh", instruction);
                break;
                default:
                handle_invalid_instruction(instruction);
                break;
            }
            break;
        case 0x2:
            print_rtype("slt", instruction);
            break;
        case 0x4:
            switch (instruction.rtype.funct7) {
                case 0x0:   
                print_rtype("xor", instruction);
                break;
                case 0x1:
                print_rtype("div", instruction);
                break;
                default:
                handle_invalid_instruction(instruction);
                break;
            }
            break;
        case 0x5:
            switch (instruction.rtype.funct7) {
                case 0x0:
                print_rtype("srl", instruction);
                break;
                case 0x20:
                print_rtype("sra", instruction);
                break;
                default:
                handle_invalid_instruction(instruction);
                break;
            }
            break;
        case 0x6:
            switch (instruction.rtype.funct7) {
                case 0x0:
                print_rtype("or", instruction);
                break;
                case 0x1:
                print_rtype("rem", instruction);
                break;
                default:
                handle_invalid_instruction(instruction);
                break;
            }
            break;
        case 0x7:
            print_rtype("and", instruction);
            break;
        default:
            handle_invalid_instruction(instruction);
        break;
    }
}

void write_itype_except_load(Instruction instruction) {
    int shiftOp;
    switch (instruction.itype.funct3) {
        case 0x0:
            print_itype_except_load("addi", instruction, instruction.itype.imm);
            break;
        case 0x1:
            print_itype_except_load("slli", instruction, instruction.itype.imm);
            break;
        case 0x2:
            print_itype_except_load("slti", instruction, instruction.itype.imm);
            break;
        case 0x4:
            print_itype_except_load("xori", instruction, instruction.itype.imm);
            break;
        case 0x5:
            shiftOp = instruction.itype.imm >> 10;
            switch(shiftOp) {
                case 0x0:
                    print_itype_except_load("srli", instruction, instruction.itype.imm & 0x1F);
                    break;
                case 0x1:
                    print_itype_except_load("srai", instruction, instruction.itype.imm & 0x1F);
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    break;
            }
            break;
        case 0x6:
            print_itype_except_load("ori", instruction, instruction.itype.imm);
            break;
        case 0x7:
            print_itype_except_load("andi", instruction, instruction.itype.imm);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;  
    }
}

void write_load(Instruction instruction) {
    switch (instruction.itype.funct3) {
        case 0x0:
            print_load("lb", instruction);
            break;
        case 0x1:
            print_load("lh", instruction);
            break;
        case 0x2:
            print_load("lw", instruction);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

void write_store(Instruction instruction) {
    switch (instruction.stype.funct3) {
        case 0x0:
            print_store("sb", instruction);
            break;
        case 0x1:
            print_store("sh", instruction);
            break;
        case 0x2:
            print_store("sw", instruction);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

void write_branch(Instruction instruction) {
    switch (instruction.sbtype.funct3) {
        case 0x0:
            print_branch("beq", instruction);
            break;
        case 0x1:
            print_branch("bne", instruction);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

void write_custom(Instruction instruction) {
    switch (instruction.rtype.funct3) {
        case 0x0:
            print_custom("mac", instruction);
            break;
        case 0x1:
            print_custom("acc", instruction);
            break;
        case 0x2:
            print_custom("gep", instruction);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

// bash ./scripts/localci.sh

// ./riscv -d ./code/input/R/R.input > ./code/out/R/R.solution
// diff ./code/out/R/R.solution ./code/ref/R/R.solution

// ./riscv -d ./code/input/I/I.input > ./code/out/I/I.solution
// diff ./code/out/I/I.solution ./code/ref/I/I.solution

// ./riscv -d ./code/input/S/S.input > ./code/out/S/S.solution
// diff ./code/out/S/S.solution ./code/ref/S/S.solution

// ./riscv -d ./code/input/SB/SB.input > ./code/out/SB/SB.solution
// diff ./code/out/SB/SB.solution ./code/ref/SB/SB.solution

// ./riscv -d ./code/input/U/U.input > ./code/out/U/U.solution
// diff ./code/out/U/U.solution ./code/ref/U/U.solution

// ./riscv -d ./code/input/UJ/UJ.input > ./code/out/UJ/UJ.solution
// diff ./code/out/UJ/UJ.solution ./code/ref/UJ/UJ.solution
// You can replace R with any of the other instruction types R, I, S, SB, U, J

void print_lui(Instruction instruction) {
    printf(LUI_FORMAT, instruction.utype.rd, instruction.utype.imm);
}

void print_jal(Instruction instruction) {
    int offset = get_jump_offset(instruction);
    printf(JAL_FORMAT, instruction.ujtype.rd, offset);
}

void print_ecall(Instruction instruction) {
    printf(ECALL_FORMAT);
}

void print_rtype(char *name, Instruction instruction) {
    printf(RTYPE_FORMAT, name, instruction.rtype.rd, instruction.rtype.rs1, instruction.rtype.rs2);
}

void print_itype_except_load(char *name, Instruction instruction, int imm) {
    printf(ITYPE_FORMAT, name, instruction.itype.rd, instruction.itype.rs1, sign_extend_number((unsigned int)imm, 12U));
        // printf(ITYPE_FORMAT, name, instruction.itype.rd, instruction.itype.rs1, imm);
}

void print_load(char *name, Instruction instruction) {
    printf(MEM_FORMAT, name, instruction.itype.rd, sign_extend_number((unsigned int)instruction.itype.imm, 12U), instruction.itype.rs1);
    // printf(MEM_FORMAT, name, instruction.itype.rd, instruction.itype.imm, instruction.itype.rs1);
}

void print_store(char *name, Instruction instruction) {
    int offset = get_store_offset(instruction);
    printf(MEM_FORMAT, name, instruction.stype.rs2, offset, instruction.stype.rs1);
}

void print_branch(char *name, Instruction instruction) {
    int offset = get_branch_offset(instruction);
    printf(BRANCH_FORMAT, name, instruction.sbtype.rs1, instruction.sbtype.rs2, offset);
}

void print_custom(char* name, Instruction instruction) {
    printf(RTYPE_FORMAT, name, instruction.rtype.rd, instruction.rtype.rs1, instruction.rtype.rs2);
}
