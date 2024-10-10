#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"
#include "riscv.h"

void execute_rtype(Instruction, Processor *);
void execute_itype_except_load(Instruction, Processor *);
void execute_branch(Instruction, Processor *);
void execute_jal(Instruction, Processor *);
void execute_load(Instruction, Processor *, Byte *);
void execute_store(Instruction, Processor *, Byte *);
void execute_ecall(Processor *, Byte *);
void execute_lui(Instruction, Processor *);
void execute_custom(Instruction, Processor *);

void execute_instruction(uint32_t instruction_bits, Processor *processor,Byte *memory) {    
    Instruction instruction = parse_instruction(instruction_bits);
    switch(instruction.opcode) {
        case 0x33:
            execute_rtype(instruction, processor);
            break;
        case 0x13:
            execute_itype_except_load(instruction, processor);
            break;
        case 0x73:
            execute_ecall(processor, memory);
            break;
        case 0x63:
            execute_branch(instruction, processor);
            break;
        case 0x6F:
            execute_jal(instruction, processor);
            break;
        case 0x23:
            execute_store(instruction, processor, memory);
            break;
        case 0x03:
            execute_load(instruction, processor, memory);
            break;
        case 0x37:
            execute_lui(instruction, processor);
            break;
        case 0x2b:
            execute_custom(instruction, processor);
            break;
        default: // undefined opcode
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_rtype(Instruction instruction, Processor *processor) {
    switch (instruction.rtype.funct3) {
        case 0x0:
            switch (instruction.rtype.funct7) {
                case 0x0:
                  // Add
                    processor->R[instruction.rtype.rd] =
                        ((sWord)processor->R[instruction.rtype.rs1]) +
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    processor->PC += 4;
                    break;
                case 0x1:
                  // Mul
                    processor->R[instruction.rtype.rd] =
                        ((sWord)processor->R[instruction.rtype.rs1]) *
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    processor->PC += 4;
                    break;
                case 0x20:
                    // Sub
                    processor->R[instruction.rtype.rd] = 
                        ((sWord)processor->R[instruction.rtype.rs1]) -
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    processor->PC += 4;
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                    break;
            }
            break;
        case 0x1:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // SLL
                    processor->R[instruction.rtype.rd] =
                        ((sWord)processor->R[instruction.rtype.rs1]) <<
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    processor->PC += 4;
                    break;
                case 0x1:
                    // MULH
                    processor->R[instruction.rtype.rd] = (sWord)((((sDouble)processor->R[instruction.rtype.rs1]) * ((sDouble)processor->R[instruction.rtype.rs2])) >> 32);
                    processor->PC += 4;
                    break;
            }
            break;
        case 0x2:
            // SLT
            processor->R[instruction.rtype.rd] =
                (((sWord)processor->R[instruction.rtype.rs1]) <
                ((sWord)processor->R[instruction.rtype.rs2])) ? 1 : 0;
            processor->PC += 4;
            break;
        case 0x4:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // XOR
                    processor->R[instruction.rtype.rd] =
                        ((sWord)processor->R[instruction.rtype.rs1]) ^
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    processor->PC += 4;
                    break;
                case 0x1:
                    // DIV
                    processor->R[instruction.rtype.rd] =
                        ((sWord)processor->R[instruction.rtype.rs1]) /
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    processor->PC += 4;
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                    break;
            }
            break;
        case 0x5:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // SRL
                    processor->R[instruction.rtype.rd] =
                        ((Word)processor->R[instruction.rtype.rs1]) >>
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    processor->PC += 4;      
                    break;
                case 0x20:
                    // SRA
                    processor->R[instruction.rtype.rd] =
                        ((sWord)processor->R[instruction.rtype.rs1]) >>
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    processor->PC += 4;  
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                break;
            }
            break;
        case 0x6:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // OR
                    processor->R[instruction.rtype.rd] =
                        ((sWord)processor->R[instruction.rtype.rs1]) |
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    processor->PC += 4;  
                    break;
                case 0x1:
                    // REM
                    processor->R[instruction.rtype.rd] =
                        ((sWord)processor->R[instruction.rtype.rs1]) %
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    processor->PC += 4;  
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                    break;
            }
            break;
        case 0x7:
            // AND
            processor->R[instruction.rtype.rd] =
                ((sWord)processor->R[instruction.rtype.rs1]) &
                ((sWord)processor->R[instruction.rtype.rs2]);
            processor->PC += 4;
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_itype_except_load(Instruction instruction, Processor *processor) {
    switch (instruction.itype.funct3) {
        case 0x0:
            // ADDI
            processor->R[instruction.itype.rd] =
                ((sWord)processor->R[instruction.itype.rs1]) +
                (sign_extend_number(instruction.itype.imm, 12));
            processor->PC += 4;  
            break;
        case 0x1:
            // SLLI
            processor->R[instruction.itype.rd] =
                ((sWord)processor->R[instruction.itype.rs1]) <<
                ((sWord)(instruction.itype.imm & 0x1F));
            processor->PC += 4;  
            break;
        case 0x2:
            // STLI
            processor->R[instruction.itype.rd] = 
                ((sWord)processor->R[instruction.itype.rs1]) <
                (sign_extend_number(instruction.itype.imm, 12)) ? 1U : 0U;
            processor->PC += 4;  
            break;
        case 0x4:
            // XORI
            processor->R[instruction.itype.rd] =
                ((sWord)processor->R[instruction.itype.rs1]) ^
                (instruction.itype.imm);
            processor->PC += 4;  
            break;
        case 0x5:
            // Shift Right (You must handle both logical and arithmetic)
            switch ((instruction.itype.imm >> 5) & 0x7F) {
                case 0x00:
                    processor->R[instruction.itype.rd] =
                        ((Word)processor->R[instruction.itype.rs1]) >>
                        ((sWord)(instruction.itype.imm & 0x1F));
                    processor->PC += 4;  
                    break;
                case 0x20:
                    processor->R[instruction.itype.rd] =
                        ((sWord)processor->R[instruction.itype.rs1]) >>
                        ((sWord)instruction.itype.imm & 0x1F);
                    processor->PC += 4;  
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                    break;
            }
            break;
        case 0x6:
            // ORI
            processor->R[instruction.itype.rd] =
                ((sWord)processor->R[instruction.itype.rs1]) |
                (sign_extend_number(instruction.itype.imm, 12));
            processor->PC += 4;  
            break;
        case 0x7:
            // ANDI
            processor->R[instruction.itype.rd] =
                ((sWord)processor->R[instruction.itype.rs1]) &
                (sign_extend_number(instruction.itype.imm, 12));
            processor->PC += 4;  
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

void execute_ecall(Processor *p, Byte *memory) {
    Register i;
    
    // syscall number is given by a0 (x10)
    // argument is given by a1
    switch(p->R[10]) {
        case 1: // print an integer
            printf("%d",p->R[11]);
            break;
        case 4: // print a string
            for(i=p->R[11];i<MEMORY_SPACE && load(memory,i,LENGTH_BYTE);i++) {
                printf("%c",load(memory,i,LENGTH_BYTE));
            }
            break;
        case 10: // exit
            printf("exiting the simulator\n");
            exit(0);
            break;
        case 11: // print a character
            printf("%c",p->R[11]);
            break;
        default: // undefined ecall
            printf("Illegal ecall number %d\n", p->R[10]);
            exit(-1);
            break;
    }
    p->PC += 4;
}

void execute_branch(Instruction instruction, Processor *processor) {
    switch (instruction.sbtype.funct3) {
        case 0x0:
            // BEQ
            if (processor->R[instruction.sbtype.rs1] == processor->R[instruction.sbtype.rs2]) {
                processor->PC += get_branch_offset(instruction);
            } else {
                processor->PC += 4;
            }
            break;
        case 0x1:
            // BNE
            if (processor->R[instruction.sbtype.rs1] != processor->R[instruction.sbtype.rs2]) {
                processor->PC += get_branch_offset(instruction);
            } else {
                processor->PC += 4;
            }
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_load(Instruction instruction, Processor *processor, Byte *memory) {
    switch (instruction.itype.funct3) {
        int data;
        case 0x0:
            // LB
            data = load(
                memory,
                sign_extend_number(((sWord)(instruction.itype.imm)), 12) + ((sWord)processor->R[(instruction.itype.rs1)]),
                LENGTH_BYTE
            );

            processor->R[instruction.itype.rd] = data;
            processor->PC += 4; 
            break;
        case 0x1:
            // LH
            data = load(
                memory,
                sign_extend_number(((sWord)(instruction.itype.imm)), 12) + ((sWord)processor->R[(instruction.itype.rs1)]),
                LENGTH_HALF_WORD
            );

            processor->R[instruction.itype.rd] = data;
            processor->PC += 4; 
            break;
        case 0x2:
            // LW
            data = load(
                memory,
                sign_extend_number(((sWord)(instruction.itype.imm)), 12) + ((sWord)processor->R[(instruction.itype.rs1)]),
                LENGTH_WORD
            );

            processor->R[instruction.itype.rd] = sign_extend_number(data, LENGTH_WORD);
            processor->PC += 4; 
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

void execute_store(Instruction instruction, Processor *processor, Byte *memory) {
    switch (instruction.stype.funct3) {
        case 0x0:
            // SB
            store(
                memory,
                get_store_offset(instruction) + (sWord)processor->R[instruction.stype.rs1],
                LENGTH_BYTE,
                (Word)processor->R[instruction.stype.rs2]
            );
            processor->PC += 4;
            break;
        case 0x1:
            // SH
            store(
                memory,
                get_store_offset(instruction) + (sWord)processor->R[instruction.stype.rs1],
                LENGTH_HALF_WORD,
                (Word)processor->R[instruction.stype.rs2]
            );
            processor->PC += 4;
            break;
        case 0x2:
            // SW
            store(
                memory,
                get_store_offset(instruction) + (sWord)processor->R[instruction.stype.rs1],
                LENGTH_WORD,
                (Word)processor->R[instruction.stype.rs2]
            );
            processor->PC += 4;
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_jal(Instruction instruction, Processor *processor) {
    /* YOUR CODE HERE */
    processor->R[instruction.ujtype.rd] = processor->PC + 4;
    processor->PC += get_jump_offset(instruction);
}

void execute_lui(Instruction instruction, Processor *processor) {
    // processor->R[instruction.utype.rd] = ((sWord)processor->R[instruction.utype.imm]) << 11;
    // processor->PC += 4; 
    int x = instruction.utype.rd;
    int upperImm = ((sWord)instruction.utype.imm) << 12;
    processor->R[x] = upperImm;
    processor->PC += 4;
}

void execute_custom(Instruction instruction, Processor *processor) {
    switch(instruction.rtype.funct3) {
        case 0x0:
          // Mac
            processor->R[instruction.rtype.rd] = 
                ((sWord)processor->R[instruction.rtype.rd]) +
                (((sWord)processor->R[instruction.rtype.rs1]) *
                ((sWord)processor->R[instruction.rtype.rs2]));
            processor->PC += 4;
            break;
        case 0x1:
          // Acc
            processor->R[instruction.rtype.rd] = 
                ((sWord)processor->R[instruction.rtype.rd]) +
                (((sWord)processor->R[instruction.rtype.rs1]) +
                ((sWord)processor->R[instruction.rtype.rs2]));
            processor->PC += 4;
            break;
        case 0x2:
          // Gep
            processor->R[instruction.rtype.rd] = 
                ((sWord)processor->R[instruction.rtype.rs1]) +
                (((sWord)processor->R[instruction.rtype.rs2]) << 4);
            processor->PC += 4;
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }   
}

void store(Byte *memory, Address address, Alignment alignment, Word value) {
    /* YOUR CODE HERE */
    Byte* addr = memory + address;

    uint8_t chunk1 = value & 0xFF;
    uint8_t chunk2 = (value >> 8) & 0xFF;
    uint8_t chunk3 = (value >> 16) & 0xFF;
    uint8_t chunk4 = (value >> 24) & 0xFF;

    if (alignment == 4) {
        *(addr) = chunk1;
        *(addr + 1) = chunk2;
        *(addr + 2) = chunk3;
        *(addr + 3) = chunk4; 
    } else if (alignment == 2) {
        *(addr) = chunk1;
        *(addr + 1) = chunk2;
    } else {
        *(addr) = chunk1;
    }

}

Word load(Byte *memory, Address address, Alignment alignment) {
    /* YOUR CODE HERE */
    void* addr = memory + address;
    Word val = 0;

    if (alignment == 4) {
        val = *((sWord*) addr);
    } else if (alignment == 2) {
        val = *((sHalf*) addr);
    } else {
        val = *((sByte*) addr);
    }
    return val;
}
