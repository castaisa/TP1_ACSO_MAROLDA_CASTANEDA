#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "shell.h"

#define OPCODE_TABLE_SIZE 50
#define MASK_21 0x7FF
#define MASK_26 0x3F
#define MASK_24 0xFF
#define MASK_22 0x3FF
#define MASK_9bits 0x1FF
#define MASK_5bits 0x1F
#define MASK_11bits 0xFFF
#define MASK_16bits 0xFFFF
#define MASK_19bits 0x7FFFF
#define MASK_26bits 0x3FFFFFFF
#define MASK_2bits 0x3

typedef struct instruction_t{
    uint32_t opcode;
    uint32_t rd;
    uint32_t rn;
    uint32_t rm;
    uint32_t rt;
    uint32_t shamt;
    uint32_t alu_immediate;
    uint32_t dt_address;
    uint32_t br_address;
    uint32_t cond_br_address;
    uint32_t mov_immediate;
    char type[20];  
    char *name;
} instruction;

void process_instruction();
instruction decode_instruction(uint32_t bytecode);
void decode_instruction_opcode(instruction *instr, uint32_t bytecode);
void decode_completely_instruction(instruction *instr, uint32_t bytecode);

void implement_ADDS_immediate(instruction instruct);
void implement_ADDS_extended_register(instruction instruct);
void implement_SUBS_immediate(instruction instruct);
void implement_SUBS_extended_register(instruction instruct);
void implement_HLT(instruction instruct);
void implement_ANDS_shifted_register(instruction instruct);
void implement_EOR_shifted_register(instruction instruct);
void implement_MOVZ(instruction instruct);
void implement_STURB(instruction instruct);
void implement_LSL_immediate(instruction instruct);
void implement_STUR(instruction instruct);
void implement_LDUR(instruction instruct);
void implement_LDURB(instruction instruct);
void implement_BCOND(instruction instruct);
void implement_ORR_shifted_register(instruction instruct);
void implement_STURH(instruction instruct);
void implement_LDURH(instruction instruct);
void implement_AND_inmediate(instruction instruct);
void implement_SUB_inmediate(instruction instruct);
void implement_CBZ(instruction instruct);
void implement_CBNZ(instruction instruct);
void implement_B(instruction instruct);
void implement_BR(instruction instruct);
void implement_MUL(instruction instruct);
void implement_ADD_immediate(instruction instruct);
void implement_LSR_immediate(instruction instruct);
void implement_ADD_extended_register(instruction instruct);


const instruction opcode_table[OPCODE_TABLE_SIZE] = {
    {0b1010101100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "X", "ADDS(Extended Register)"},
    {0b10110001, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "I", "ADDS(immediate)"}, 
    {0b1110101100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "X", "SUBS(Extended Register)"},
    {0b11110001, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "I", "SUBS(immediate)"},
    {0b11010100010, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "CB", "HLT"},
    {0b11101010, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "X", "ANDS(Shifted Register)"},
    {0b11001010, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "X", "EOR(Shifter Register)"},
    {0b10101010000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "I", "ORR(Shifted Register)"},
    {0b000101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "B", "B"},
    {0b11010110000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "I", "BR"}, 
    {0b01010100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "CB", "BCOND"},  
    {0b11010011011, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "R", "LSL(Immediate)"},
    {0b11010011010, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "R", "LSR(Immediate)"},
    {0b11111000000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "D", "STUR"}, 
    {0b00111000000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "D", "STURB"},
    {0b0111100000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "D", "STURH"}, 
    {0b11111000010, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "D", "LDUR"},
    {0b01111000010, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "D", "LDURH"},
    {0b0011100001, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,"D", "LDURB"},
    {0b11010010, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "IW", "MOVZ"},
    {0b10000101101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "I", "ADD(Extended Register)"},
    {0b10010001, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "I", "ADD(immediate)"},
    {0b10011011000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "X", "MUL"},
    {0b10110100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "CB", "CBZ"},
    {0b10110101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "CB", "CBNZ"},
    {11010001, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "I", "SUB(immediate)"},
    {1001001000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "I", "AND(immediate)"},
};


void process_instruction(){
    uint32_t bytecode = mem_read_32(CURRENT_STATE.PC);
    instruction instruct = decode_instruction(bytecode);
    printf("Instrucción: %s\n", instruct.name);

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    if (strcmp(instruct.name, "ADDS(immediate)") == 0) implement_ADDS_immediate(instruct);
    if (strcmp(instruct.name, "ADDS(Extended Register)") == 0) implement_ADDS_extended_register(instruct);
    if (strcmp(instruct.name, "SUBS(immediate)") == 0) implement_SUBS_immediate(instruct);
    if (strcmp(instruct.name, "SUBS(Extended Register)") == 0) implement_SUBS_extended_register(instruct);
    if (strcmp(instruct.name, "HLT") == 0) implement_HLT(instruct);
    if (strcmp(instruct.name, "ANDS(Shifted Register)") == 0) implement_ANDS_shifted_register(instruct);
    if (strcmp(instruct.name, "EOR(Shifter Register)") == 0 ) implement_EOR_shifted_register(instruct);
    if (strcmp(instruct.name, "ORR(Shifted Register)") == 0) implement_ORR_shifted_register(instruct);
    if (strcmp(instruct.name, "B") == 0) implement_B(instruct);
    if (strcmp(instruct.name, "BCOND") == 0) implement_BCOND(instruct);
    if (strcmp(instruct.name, "BR") == 0) implement_BR(instruct);
    if (strcmp(instruct.name, "LSL(Immediate)") == 0) implement_LSL_immediate(instruct);
    if (strcmp(instruct.name, "LSR(Immediate)") == 0) implement_LSR_immediate(instruct);
    if (strcmp(instruct.name, "STUR") == 0) implement_STUR(instruct);
    if (strcmp(instruct.name, "STURB") == 0) implement_STURB(instruct);
    if (strcmp(instruct.name, "STURH") == 0) implement_STURH(instruct);
    if (strcmp(instruct.name, "LDUR") == 0) implement_LDUR(instruct);
    if (strcmp(instruct.name, "LDURB") == 0) implement_LDURB(instruct);
    if (strcmp(instruct.name, "LDURH") == 0) implement_LDURH(instruct);
    if (strcmp(instruct.name, "MOVZ") == 0) implement_MOVZ(instruct);
    if (strcmp(instruct.name, "ADD(Extended Register)") == 0) implement_ADD_extended_register(instruct);
    if (strcmp(instruct.name, "ADD(immediate)") == 0) implement_ADD_immediate(instruct);
    if (strcmp(instruct.name, "MUL") == 0) implement_MUL(instruct);
    if (strcmp(instruct.name, "CBZ") == 0 ) implement_CBZ(instruct);
    if (strcmp(instruct.name, "CBNZ") == 0) implement_CBNZ(instruct);
}

instruction decode_instruction(uint32_t bytecode) {
    instruction instr_def;

    decode_instruction_opcode(&instr_def, bytecode);
    decode_completely_instruction(&instr_def, bytecode);

    return instr_def;
}


void decode_instruction_opcode(instruction *instr, uint32_t bytecode) {
    
    // Instrucciones tipo R, D, IW (Opcode en bits [31:21], 11 bits)
    uint32_t opcode_21 = (bytecode >> 21) & MASK_21;  
    printf("Opcode 21: 0x%X\n", opcode_21);
    for (int i = 0; i < OPCODE_TABLE_SIZE; i++) {
        if (opcode_table[i].opcode == opcode_21) {
            printf("Entro al opcode 21\n");
            instr->opcode = opcode_table[i].opcode;
            instr->name = opcode_table[i].name;
            strcpy(instr->type, opcode_table[i].type);
            return;
        }
    }

    // Instrucciones tipo B (Opcode en bits [31:26], 6 bits)
    uint32_t opcode_26 = (bytecode >> 26) & MASK_26;
    printf("Opcode 26: 0x%X\n", opcode_26);
    for (int i = 0; i < OPCODE_TABLE_SIZE; i++) {
        if (opcode_table[i].opcode == opcode_26) {
            printf("Entro al opcode 26\n");
            instr->opcode = opcode_table[i].opcode;
            instr->name = opcode_table[i].name;
            strcpy(instr->type, opcode_table[i].type);
            return;
        }
    }

    // Instrucciones tipo CB (Opcode en bits [31:24], 8 bits)
    uint32_t opcode_24 = (bytecode >> 24) & MASK_24;
    printf("Opcode 24: 0x%X\n", opcode_24);
    for (int i = 0; i < OPCODE_TABLE_SIZE; i++) {
        if (opcode_table[i].opcode == opcode_24) {
            printf("Entro al opcode 24\n");
            instr->opcode = opcode_table[i].opcode;
            instr->name = opcode_table[i].name;
            strcpy(instr->type, opcode_table[i].type);
            return;
        }
    }

    // Instrucciones tipo I (Immediate) - Opcode en bits [31:22], 10 bits
    uint32_t opcode_22 = (bytecode >> 22) & MASK_22;  
    printf("Opcode 22: 0x%X\n", opcode_22);
    for (int i = 0; i < OPCODE_TABLE_SIZE; i++) {
        if (opcode_table[i].opcode == opcode_22) {
            printf("Entro al opcode 22\n");
            instr->opcode = opcode_table[i].opcode;
            instr->name = opcode_table[i].name;
            strcpy(instr->type, opcode_table[i].type);
            return;
        }
    }
    
}

void decode_completely_instruction(instruction *instr, uint32_t bytecode) {
     if (strcmp(instr->type, "I") == 0) {
        instr->rd = bytecode & MASK_5bits;             // Bits [4:0] - Registro de destino
        instr->rn = (bytecode >> 5) & MASK_5bits;      // Bits [9:5] - Registro fuente
        instr->alu_immediate = (bytecode >> 10) & MASK_11bits;  // Bits [21:10] - Inmediato ALU
        instr->shamt = (bytecode >> 22) & MASK_2bits;   // Bits [23:22] - Shift amount
        strcpy(instr->type, "I");
    } else if (strcmp(instr->type, "X") == 0) {
        instr->rd = bytecode & MASK_5bits;             // Bits [4:0] - Registro de destino
        instr->rn = (bytecode >> 5) & MASK_5bits;      // Bits [9:5] - Registro fuente
        instr->rm = (bytecode >> 16) & MASK_5bits;     // Bits [20:16] - Registro fuente 2
        strcpy(instr->type, "X");
    } else if (strcmp(instr->type, "IW") == 0) {
        instr->rd = bytecode & MASK_5bits;                 // Bits [4:0] - Registro de destino
        instr->mov_immediate = (bytecode >> 5) & MASK_16bits;  // Bits [20:5] - Inmediato MOV
        instr->shamt = (bytecode >> 21) & MASK_2bits;       // Bits [22:21] - Shift amount (hw)
        strcpy(instr->type, "IW");
    } else if (strcmp(instr->type, "R") == 0) {
        instr->rd = bytecode & MASK_5bits;               // Bits [4:0] - Registro destino
        instr->rn = (bytecode >> 5) & MASK_5bits;        // Bits [9:5] - Registro fuente
        instr->shamt = (bytecode >> 10) & MASK_26;     // Bits [15:10] - immr
        instr->rm = (bytecode >> 16) & MASK_26;     // Bits [21:16] - imms
        strcpy(instr->type, "R");
        
    } 
    // else if (strcmp(instr->type, "G5") == 0) {
    //     instr->rd = bytecode & MASK_5bits;               // Bits [4:0] - Registro destino
    //     instr->rn = (bytecode >> 5) & MASK_5bits;        // Bits [9:5] - Registro fuente
    //     instr->imms = (bytecode >> 10) & MASK_26;     // Bits [15:10] - immr
    //     instr->immr = (bytecode >> 16) & MASK_26;     // Bits [21:16] - imms
    //     strcpy(instr->type, "G5");
    // } 
    else if (strcmp(instr->type, "CB") == 0) {
        instr->cond_br_address = (bytecode >> 5) & MASK_19bits;     // Bits [23:5] - Dirección de salto condicional
        instr->rt = (bytecode) & MASK_5bits;  // Bits [5:0] - Dirección de salto condicional
        strcpy(instr->type, "CB");
    } else if (strcmp(instr->type, "B") == 0) {
        instr->br_address = (bytecode) & MASK_26;   // Bits [25:] - Dirección de salto// Bits [25:0] - Dirección de salto
        strcpy(instr->type, "B");
    } else if (strcmp(instr->type, "D") == 0) {
        instr->rd = bytecode & MASK_5bits;             // Bits [4:0] - Registro de destino
        instr->rn = (bytecode >> 5) & MASK_5bits;      // Bits [9:5] - Registro fuente
        instr->dt_address = (bytecode >> 12) & MASK_9bits;   // Bits [20:12] - Dirección de datos
        strcpy(instr->type, "D");
    }
}

// INSTRUCCIONES -------------------------------------------------------------------------------------------
void implement_ADDS_immediate(instruction instruct) {
    printf("Implementing ADDS(immediate)\n");

    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn];
    uint64_t op2 = instruct.alu_immediate;

    uint64_t result = op1 + op2;
    NEXT_STATE.REGS[instruct.rd] = result;
   
    NEXT_STATE.FLAG_N = (result >> 63) & 1;
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;
}

void implement_ADDS_extended_register(instruction instruct) {
    printf("Implementing ADDS(Extended Register)\n");
    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn]; 
    uint64_t op2 = CURRENT_STATE.REGS[instruct.rm]; 

    uint64_t result = op1 + op2;
    
    NEXT_STATE.REGS[instruct.rd] = result; 

    NEXT_STATE.FLAG_N = (result >> 63) & 1;
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;
    
}

void implement_SUBS_immediate(instruction instruct) {
    printf("Implementing SUBS(immediate)\n");

    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn];
    uint64_t op2 = instruct.alu_immediate;

    uint64_t result = op1 - op2;
    
    NEXT_STATE.FLAG_N = (result >> 63) & 1; 
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; 
    
    if (instruct.rd != 31) {
        NEXT_STATE.REGS[instruct.rd] = result; 
       
    }
}

void implement_SUBS_extended_register(instruction instruct) {
    printf("Implementing SUBS(Extended Register)\n");

    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn]; 
    uint64_t op2 = CURRENT_STATE.REGS[instruct.rm];

    uint64_t result = op1 - op2;

    NEXT_STATE.FLAG_N = (result >> 63) & 1;
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; 

    if (instruct.rd != 31) {
        NEXT_STATE.REGS[instruct.rd] = result; 
        
    }
}

void implement_HLT(instruction instruct) {
    printf("Implementing HLT\n");
    RUN_BIT = 0;
}

void implement_ANDS_shifted_register(instruction instruct) {
    printf("Implementing ANDS(Shifted Register)\n");

    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn];  
    uint64_t op2 = CURRENT_STATE.REGS[instruct.rm]; 

    uint64_t result = op1 & op2;
   
    NEXT_STATE.REGS[instruct.rd] = result;

    NEXT_STATE.FLAG_N = (result >> 63) & 1; 
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; 
    
}

void implement_MOVZ(instruction instruct) {
    printf("Implementing MOVZ\n");

    uint64_t imm = instruct.mov_immediate; 

    uint64_t result = imm;
    
    NEXT_STATE.REGS[instruct.rd] = result; 

    NEXT_STATE.FLAG_N = (result >> 63) & 1; 
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; 
    
}

void implement_LSL_immediate(instruction instruct) {
    printf("Implementing LSL(Immediate)\n");

    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn];
    uint64_t shift_amount = 64 - instruct.rm; 

    uint64_t result = op1 << shift_amount;

    NEXT_STATE.REGS[instruct.rd] = result;

    NEXT_STATE.FLAG_N = (result >> 63) & 1;  
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; 
}

void implement_STUR(instruction instruct) {
    printf("Implementing STUR\n");
    uint64_t signed_offset;
    if (instruct.dt_address & (1 << 8)) {  
        signed_offset = (uint64_t)(instruct.dt_address | 0xFFFFFFFFFFFFFF00);
    } else {
        signed_offset = (uint64_t)(instruct.dt_address & 0x1FF); 
    }

    uint64_t address = CURRENT_STATE.REGS[instruct.rn] + signed_offset;
    uint32_t value = CURRENT_STATE.REGS[instruct.rd] & 0xFFFFFFFF;

    mem_write_32(address, value);
   
}

void implement_STURB(instruction instruct) {
    printf("Implementing STURB\n");

     uint64_t signed_offset;
    if (instruct.dt_address & (1 << 8)) { 
        signed_offset = (uint64_t)(instruct.dt_address | 0xFFFFFFFFFFFFFF00);
    } else {
        signed_offset = (uint64_t)(instruct.dt_address & 0x1FF);
    }

    uint64_t address = CURRENT_STATE.REGS[instruct.rn] + signed_offset;

    uint8_t value = CURRENT_STATE.REGS[instruct.rd] & 0xFF;

    uint32_t aligned_address = address & ~0x3;
    uint32_t aligned_value = mem_read_32(aligned_address); 
    uint32_t byte_shift = (address & MASK_2bits) * 8;  
    aligned_value = (aligned_value & ~(0xFF << byte_shift)) | (value << byte_shift); 
    mem_write_32(aligned_address, aligned_value); 

}

void implement_LDUR(instruction instruct) {
    printf("Implementing LDUR\n");
    uint64_t signed_offset;
    if (instruct.dt_address & (1 << 8)) {
        signed_offset = (uint64_t)(instruct.dt_address | 0xFFFFFFFFFFFFFF00);
    } else {
        signed_offset = (uint64_t)(instruct.dt_address & 0x1FF); 
    }

    uint64_t address = CURRENT_STATE.REGS[instruct.rn] + signed_offset;

    uint32_t value = mem_read_32(address);
    
    uint32_t aligned_address = address & ~0x3; 
    uint32_t aligned_value = mem_read_32(aligned_address);
    uint32_t aligned_high = mem_read_32(aligned_address + 4);

    uint64_t concatenado = (uint64_t)aligned_high << 32 | aligned_value;
    NEXT_STATE.REGS[instruct.rd] = concatenado;
   
}

void implement_LDURB(instruction instruct) {
    printf("Implementing LDURB\n");

    uint64_t signed_offset;
    if (instruct.dt_address & (1 << 8)) { 
        signed_offset = (uint64_t)(instruct.dt_address | 0xFFFFFFFFFFFFFF00);
    } else {
        signed_offset = (uint64_t)(instruct.dt_address & 0x1FF);
    }

    uint64_t address = CURRENT_STATE.REGS[instruct.rn] + signed_offset;

    uint32_t aligned_address = address & ~0x3; 
    uint32_t aligned_value = mem_read_32(aligned_address);
    uint32_t byte_shift = (address & MASK_2bits) * 8;
    uint8_t value = (aligned_value >> byte_shift) & 0xFF;

    NEXT_STATE.REGS[instruct.rd] = value;
}

void implement_EOR_shifted_register(instruction instruct) {
    printf("Implementing EOR(Shifter Register)\n");

    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn];
    uint64_t op2 = CURRENT_STATE.REGS[instruct.rm];

    uint64_t result = op1 ^ op2;
   
    NEXT_STATE.REGS[instruct.rd] = result; 
    
}

void implement_BCOND(instruction instruct) {
    printf("Implementing BCOND\n");

    int cond = instruct.rt;
    int branch = 0;
    switch (cond) {
        case 0b0000:
            printf("BEQ\n");
            branch = CURRENT_STATE.FLAG_Z;
            break;
        case 0b0001:
            printf("BNE\n");
            branch = !CURRENT_STATE.FLAG_Z;
            break;
        case 0b1100:
            printf("BGT\n");
            branch = !CURRENT_STATE.FLAG_Z && !CURRENT_STATE.FLAG_N;
            break;
        case 0b1011:
            printf("BLT\n");
            branch = CURRENT_STATE.FLAG_N;
            break;
        case 0b1010:
            printf("BGE\n");
            branch = !CURRENT_STATE.FLAG_N;
            break;
        case 0b1101:
            printf("BLE\n");
            branch = CURRENT_STATE.FLAG_Z || CURRENT_STATE.FLAG_N;
            break;
    }

    
    if (branch) {
        int64_t signed_offset;
        if (instruct.cond_br_address & (1 << 18)) { 
            signed_offset = (int64_t)(instruct.cond_br_address | 0xFFFFFFFFFFFC0000);
        } else {
            signed_offset = (int64_t)(instruct.cond_br_address & 0x3FFFF); 
        }

        NEXT_STATE.PC = CURRENT_STATE.PC + (signed_offset << 2);
       
    }
}

void implement_ORR_shifted_register(instruction instruct) {
    printf("Implementing ORR(Shifter Register)\n");

    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn];
    uint64_t op2 = CURRENT_STATE.REGS[instruct.shamt];

    uint64_t result = op1 | op2;
    NEXT_STATE.REGS[instruct.rd] = result;
}

void implement_STURH(instruction instruct) {
    printf("Implementing STURH\n");

    uint64_t signed_offset;
    if (instruct.dt_address & (1 << 8)) { 
        signed_offset = (uint64_t)(instruct.dt_address | 0xFFFFFFFFFFFFFF00);
    } else {
        signed_offset = (uint64_t)(instruct.dt_address & 0x1FF); 
    }

    uint64_t address = CURRENT_STATE.REGS[instruct.rn] + signed_offset; 
    uint16_t value = CURRENT_STATE.REGS[instruct.rd] & 0xFFFF; 

    uint32_t aligned_address = address & ~0x3; 
    uint32_t aligned_value = mem_read_32(aligned_address); 
    uint32_t halfword_shift = (address & 0x2) * 8; 

    
    aligned_value = (aligned_value & ~(0xFFFF << halfword_shift)) | (value << halfword_shift);
    mem_write_32(aligned_address, aligned_value); 

}

void implement_LDURH(instruction instruct) {
    printf("Implementing LDURH\n");

    uint64_t signed_offset;
    if (instruct.dt_address & (1 << 8)) { 
        signed_offset = (uint64_t)(instruct.dt_address | 0xFFFFFFFFFFFFFF00); 
    } else {
        signed_offset = (uint64_t)(instruct.dt_address & 0x1FF); 
    }

    uint64_t address = CURRENT_STATE.REGS[instruct.rn] + signed_offset;

    
    uint32_t aligned_address = address & ~0x3;  
    uint32_t aligned_value = mem_read_32(aligned_address); 
    uint32_t halfword_shift = (address & 0x2) * 8; 

   
    uint16_t loaded_value = (aligned_value >> halfword_shift) & 0xFFFF;
    CURRENT_STATE.REGS[instruct.rd] = loaded_value;  
}


void implement_CBZ(instruction instruct) {
    printf("Implementing CBZ\n");

    
    int zero = 0;
    if (CURRENT_STATE.REGS[instruct.rt] == 0) {
        zero = 1; 
    } else {
        zero = 0; 
    }

    if (zero == 1) {
        uint64_t signed_offset;

    
        if (instruct.cond_br_address & (1 << 18)) {
            signed_offset = (uint64_t)(instruct.cond_br_address | 0xFFFFFFFFFFFC0000); 
        } else {
            signed_offset = (uint64_t)(instruct.cond_br_address & 0x3FFFF);
        }

        uint64_t address = CURRENT_STATE.PC + (signed_offset << 2);
       

        
        if (address % 4 != 0) {
            
            return;
        }

        
        NEXT_STATE.PC = address;

        
    } else {
        
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
void implement_CBNZ(instruction instruct) {
    printf("Implementing CBNZ\n");

    
    int zero = 0; 
    if (CURRENT_STATE.REGS[instruct.rt] == 0) {
        zero = 1; 
    } else {
        zero = 0; 
    }
    if (zero == 0) {
        uint64_t signed_offset;

        
        if (instruct.br_address & (1 << 18)) {
            signed_offset = (uint64_t)(instruct.cond_br_address | 0xFFFFFFFFFFFC0000);  
        } else {
            signed_offset = (uint64_t)(instruct.cond_br_address & 0x3FFFF); 
        }
        
        uint64_t address = CURRENT_STATE.PC + (signed_offset << 2);

        
        if (address % 4 != 0) {
            return;
        }

       
        NEXT_STATE.PC = address;

        
    } else {
        
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}

void implement_B(instruction instruct) {
    printf("Implementing B\n");

    int64_t signed_offset;
    if (instruct.br_address & (1 << 18)) {
        signed_offset = (int64_t)(instruct.br_address | 0xFFFFFFFFFFFC0000);
    } else {
        signed_offset = (int64_t)(instruct.br_address & 0x3FFFF); 
    }

    uint64_t address = CURRENT_STATE.PC + (signed_offset << 2);

    NEXT_STATE.PC = address;
}

void implement_BR(instruction instruct) {
    printf("Implementing BR\n");

    uint64_t address = CURRENT_STATE.REGS[instruct.rn];

    if (address > 0x1000000000000) { 
        printf("Warning: Jump address 0x%" PRIx64 " is out of bounds! Address too high.\n", address);
    }

    if (address % 4 != 0) {
        printf("Error: Dirección no alineada 0x%" PRIx64 "\n", address);
        return;
    }

    NEXT_STATE.PC = address;

    printf("Branching to 0x%" PRIx64 "\n", NEXT_STATE.PC);
}

void implement_MUL(instruction instruct) {
    printf("Implementing MUL\n");

    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn]; 
    uint64_t op2 = CURRENT_STATE.REGS[instruct.rm]; 

    uint64_t result = op1 * op2;
  
    NEXT_STATE.REGS[instruct.rd] = result;

}

void implement_ADD_immediate(instruction instruct) {
    printf("Implementing ADD(immediate)\n");

    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn]; 
    uint64_t op2 = instruct.alu_immediate; 

    uint64_t result = op1 + op2;
    NEXT_STATE.REGS[instruct.rd] = result; 

}

void implement_LSR_immediate(instruction instruct) {
    printf("Implementing LSR(Immediate)\n");

    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn]; 
    uint64_t shift_amount = instruct.rm;

    uint64_t result = op1 >> shift_amount;

    NEXT_STATE.REGS[instruct.rd] = result;
}

void implement_ADD_extended_register(instruction instruct) {
    printf("Implementing ADD(Extended Register)\n");

    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn];
    uint64_t op2 = CURRENT_STATE.REGS[instruct.rm];

    uint64_t result = op1 + op2;
   
    NEXT_STATE.FLAG_N = (result >> 63) & 1; 
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; 

    if (instruct.rd != 31) {
        NEXT_STATE.REGS[instruct.rd] = result; 
       
    }
}

