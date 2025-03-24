#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "shell.h"

#define OPCODE_TABLE_SIZE 50
// #define MASK_21 0x7FF
// #define MASK_26 0x3F
// #define MASK_24 0x3F
// #define MASK_22 0x7FF
// #define MASK_21 0b00000111111111111  // 0x7FF  = 0000 0111 1111 1111 (11 bits en 1)
//#define MASK_26 0b00111111          // 0x3F   = 0011 1111 (6 bits en 1)
//#define MASK_24 0b00111111          // 0x3F   = 0011 1111 (6 bits en 1)
//#define MASK_22 0b00000111111111111 // 0x7FF  = 0000 0111 1111 1111 (11 bits en 1)

typedef struct instruction_t{
    uint32_t opcode;
    uint32_t rd;
    uint32_t rn;
    uint32_t rm;
    uint32_t shamt;
    uint32_t alu_immediate;
    uint32_t dt_address;
    uint32_t br_address;
    uint32_t cond_br_address;
    uint32_t mov_immediate;
    char type[20];  // Ahora puede almacenar "R", "I", "D", "B", "CB", "IW"
    char *name;     // Nombre de la instrucción
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
// ()DRUTS_tnemel

// void decode_instruction_with_opcode(instruction *instr);


const instruction opcode_table[OPCODE_TABLE_SIZE] = {
    // {0b10101011001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G1", "ADDS(Extended Register)"},  // pg 257
    {0b1010101100, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G2", "ADDS(Extended Register)"},  // pg 257
    {0b10110001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G1", "ADDS(immediate)"}, 
    {0b1110101100, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G2", "SUBS(Extended Register)"},
    {0b11110001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G1", "SUBS(immediate)"},
    {0b00000000000000000000011010100010, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G7", "HLT"},
    // {0b000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "CMP(Extended Register)"},
    // {0b000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "CMP(immediate)"},
    // // {1111001000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G3", "ANDS(Immediate)"}, // nose que pag
    {0b11101010, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G2", "ANDS(Shifted Register)"},  //pg 256
    // {11101011000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G3", "ANDS(Shifted Register)"},  //pg 256
    {0b11001010, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G2", "EOR(Shifter Register)"},
    // // {1011001000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G3", "ORR(Shifted Register)"},
    // {0b101010100, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G3", "ORR(Shifted Register)"},
    // {0b000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G10", "B"},
    // {0b000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G9", "BR"},
    // {0b000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "BEQ"},
    // {0b000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "BNE"},
    // {0b000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "BGT"},
    // {0b000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "BLT"},
    // {0b000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "BGE"},
    // {0b000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "BLE"},
    {0b11010011011, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G1", "LSL(Immediate)"},
    // {0b11010011010, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "LSR(Immediate)"},
    // // {11111000000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G13", "STUR"},
    // {0b11111000000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "STUR"},    // pg 236
    // // {11111000010, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G13", "STURB"},
    {0b00111000000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G13", "STURB"},   // pg 235
    // // {11111000001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G13", "STURH"},
    // {0b01111000000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "STURH"},   // pg 235
    // // {11111000000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G13", "LDUR"},
    // {0b11111000001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "LDUR"},    // pg 235
    // // {11111000010, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G13", "LDURH"},
    // {0b01111000001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "LDURH"},   // pg 235
    // // {11111000001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G13", "LDURB"},
    // {0b00111000001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G13", "LDURB"},   // pg 235
    {0b11010010, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G4", "MOVZ"},
    // {0b10000101101, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G1", "ADD(Extended Register)"},
    // {0b10010001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G1", "ADD(immediate)"},
    // {0b000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "MUL"},
    // {0b10110100, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G11", "CBZ"},
    // {0b10110101, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G11", "CBNZ"},

    // {11010001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G1", "SUB(immediate)"},
    // {1001001000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G1", "AND(immediate)"},

};


void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. 
     * */
    printf("starting-----------\n");
    uint32_t bytecode = mem_read_32(CURRENT_STATE.PC);
    instruction instruct = decode_instruction(bytecode);
    printf("Instrucción: %s\n", instruct.name);

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    // falta:
        // funcion que con el opcod complete el resto de la instruccion
        // tabla de hash con los posibles opcodes
    if (strcmp(instruct.name, "ADDS(immediate)") == 0) implement_ADDS_immediate(instruct);
    if (strcmp(instruct.name, "ADDS(Extended Register)") == 0) implement_ADDS_extended_register(instruct);
    if (strcmp(instruct.name, "SUBS(immediate)") == 0) implement_SUBS_immediate(instruct);
    if (strcmp(instruct.name, "SUBS(Extended Register)") == 0) implement_SUBS_extended_register(instruct);
    if (strcmp(instruct.name, "HLT") == 0) implement_HLT(instruct);
    // if (instruct.name == "CMP") implement_CMP(instruct);
    if (strcmp(instruct.name, "ANDS(Shifted Register)") == 0) implement_ANDS_shifted_register(instruct);
    if (strcmp(instruct.name, "EOR(Shifter Register)") == 0 ) implement_EOR_shifted_register(instruct);
    // if (instruct.name == "ORR") implement_ORR(instruct);
    // if (instruct.name == "B") implement_B(instruct);
    // if (instruct.name == "BR") implement_BR(instruct);
    // if (instruct.name == "BEQ") implement_BEQ(instruct);
    // if (instruct.name == "BNE") implement_BNE(instruct);
    // if (instruct.name == "BGT") implement_BGT(instruct);
    // if (instruct.name == "BLT") implement_BLT(instruct);
    // if (instruct.name == "BGE") implement_BGE(instruct);
    // if (instruct.name == "BLE") implement_BLE(instruct);
    if (strcmp(instruct.name, "LSL(Immediate)") == 0) implement_LSL_immediate(instruct);
    // if (instruct.name == "LSR") implement_LSR(instruct);
    // if (instruct.name == "STUR") implement_STUR(instruct);
    if (strcmp(instruct.name, "STURB") == 0) implement_STURB(instruct);
    // if (instruct.name == "STURH") implement_STURH(instruct);
    // if (instruct.name == "LDUR") implement_LDUR(instruct);
    // if (instruct.name == "LDURB") implement_LDURB(instruct);
    // if (instruct.name == "LDURH") implement_LDURH(instruct);
    if (strcmp(instruct.name, "MOVZ") == 0) implement_MOVZ(instruct);
    // if (instruct.name == "ADD") implement_ADD(instruct);
    // if (instruct.name == "MUL") implement_MUL(instruct);
    // if (instruct.name == "CBZ") implement_CBZ(instruct);
    // if (instruct.name == "CBNZ") implement_CBNZ(instruct);
    
}

instruction decode_instruction(uint32_t bytecode) {
    instruction instr_def;

    decode_instruction_opcode(&instr_def, bytecode);
    // falta con el opcode completar el resto de la info en el struct de instruction
    decode_completely_instruction(&instr_def, bytecode);

    return instr_def;
}


void decode_instruction_opcode(instruction *instr, uint32_t bytecode) {
    
    // Instrucciones tipo R, D, IW (Opcode en bits [31:21], 11 bits)
    uint32_t opcode_21 = (bytecode >> 21) & 0x7FF;  
    printf("Opcode 21: 0x%X\n", opcode_21);
    for (int i = 0; i < OPCODE_TABLE_SIZE; i++) {
        if (opcode_table[i].opcode == opcode_21) {
            instr->opcode = opcode_table[i].opcode;
            instr->name = opcode_table[i].name;
            strcpy(instr->type, opcode_table[i].type);
            return;
        }
    }

    // Instrucciones tipo B (Opcode en bits [31:26], 6 bits)
    uint32_t opcode_26 = (bytecode >> 26) & 0x3F;
    printf("Opcode 26: 0x%X\n", opcode_26);
    for (int i = 0; i < OPCODE_TABLE_SIZE; i++) {
        if (opcode_table[i].opcode == opcode_26) {
            instr->opcode = opcode_table[i].opcode;
            instr->name = opcode_table[i].name;
            strcpy(instr->type, opcode_table[i].type);
            return;
        }
    }

    // Instrucciones tipo CB (Opcode en bits [31:24], 8 bits)
    uint32_t opcode_24 = (bytecode >> 24) & 0xFF;
    printf("Opcode 24: 0x%X\n", opcode_24);
    for (int i = 0; i < OPCODE_TABLE_SIZE; i++) {
        if (opcode_table[i].opcode == opcode_24) {
            instr->opcode = opcode_table[i].opcode;
            instr->name = opcode_table[i].name;
            strcpy(instr->type, opcode_table[i].type);
            return;
        }
    }

    // Instrucciones tipo I (Immediate) - Opcode en bits [31:22], 10 bits
    uint32_t opcode_22 = (bytecode >> 22) & 0x3FF;  
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
    // falta completar el resto de la instruccion con el opcode
    // Completar el resto de la instrucción con el opcode
     if (strcmp(instr->type, "G1") == 0) {
        instr->rd = bytecode & 0x1F;             // Bits [4:0] - Registro de destino
        instr->rn = (bytecode >> 5) & 0x1F;      // Bits [9:5] - Registro fuente
        instr->alu_immediate = (bytecode >> 10) & 0xFFF;  // Bits [21:10] - Inmediato ALU
        instr->shamt = (bytecode >> 22) & 0x3;   // Bits [23:22] - Shift amount
        strcpy(instr->type, "G1");
    } else if (strcmp(instr->type, "G2") == 0) {
        instr->rd = bytecode & 0x1F;             // Bits [4:0] - Registro de destino
        instr->rn = (bytecode >> 5) & 0x1F;      // Bits [9:5] - Registro fuente
        instr->rm = (bytecode >> 16) & 0x1F;     // Bits [20:16] - Registro fuente 2
        instr->shamt = (bytecode >> 22) & 0x3;   // Bits [23:22] - Shift amount
        strcpy(instr->type, "G2");
    } else if (strcmp(instr->type, "G3") == 0) {
        instr->rd = bytecode & 0x1F;             // Bits [4:0] - Registro de destino
        instr->rn = (bytecode >> 5) & 0x1F;      // Bits [9:5] - Registro fuente
        instr->alu_immediate = (bytecode >> 10) & 0xFFF;  // Bits [21:10] - Inmediato ALU
        instr->shamt = (bytecode >> 22) & 0x3;   // Bits [23:22] - Shift amount
        strcpy(instr->type, "G3");
    } else if (strcmp(instr->type, "G4") == 0) {
        instr->rd = bytecode & 0x1F;                 // Bits [4:0] - Registro de destino
        instr->mov_immediate = (bytecode >> 5) & 0xFFFF;  // Bits [20:5] - Inmediato MOV
        instr->shamt = (bytecode >> 21) & 0x3;       // Bits [22:21] - Shift amount (hw)
        strcpy(instr->type, "G4");
    } else if (strcmp(instr->type, "G7") == 0) {
        instr->cond_br_address = (bytecode >> 5) & 0xFFFF;  // Bits [20:5] - Dirección de salto condicional
        strcpy(instr->type, "G7");
    } else if (strcmp(instr->type, "G11") == 0) {
        instr->br_address = (bytecode >> 5) & 0x7FFFF;  // Bits [23:5] - Dirección de salto
        instr->rn = bytecode & 0x1F;                    // Bits [4:0] - Registro fuente
        strcpy(instr->type, "G11");
    } else if (strcmp(instr->type, "G13") == 0) {
        instr->rd = bytecode & 0x1F;             // Bits [4:0] - Registro de destino
        instr->rn = (bytecode >> 5) & 0x1F;      // Bits [9:5] - Registro fuente
        instr->dt_address = (bytecode >> 10) & 0x1FF;  // Bits [18:10] - Dirección de datos
        strcpy(instr->type, "G13");
    }
}

// INSTRUCCIONES -------------------------------------------------------------------------------------------

void implement_ADDS_immediate(instruction instruct) {
    printf("Implementing ADDS(immediate)\n");

    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn];  // Valor del registro rn
    uint64_t op2 = instruct.alu_immediate;           // Inmediato ALU

    // Ejecutar la operación
    uint64_t result = op1 + op2;
    printf("op1: 0x%" PRIx64 ", op2: 0x%" PRIx64 ", result: 0x%" PRIx64 "\n", op1, op2, result);
    NEXT_STATE.REGS[instruct.rd] = result; // Guardar resultado en rd

    // Actualizar FLAGS (solo N y Z, porque C y V no están en CPU_State)
    NEXT_STATE.FLAG_N = (result >> 63) & 1; // N flag (negativo si el bit 63 es 1)
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; // Z flag (se activa si resultado es 0)
    printf("Updated X%d to 0x%" PRIx64 "\n", instruct.rd, NEXT_STATE.REGS[instruct.rd]);
}

void implement_ADDS_extended_register(instruction instruct) {
    printf("Implementing ADDS(Extended Register)\n");
    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn];  // Valor del registro rn
    uint64_t op2 = CURRENT_STATE.REGS[instruct.rm];  // Valor del registro rm

    // Ejecutar la operación
    uint64_t result = op1 + op2;
    printf("op1: 0x%" PRIx64 ", op2: 0x%" PRIx64 ", result: 0x%" PRIx64 "\n", op1, op2, result);
    NEXT_STATE.REGS[instruct.rd] = result; // Guardar resultado en rd

    // Actualizar FLAGS (solo N y Z, porque C y V no están en CPU_State)
    NEXT_STATE.FLAG_N = (result >> 63) & 1; // N flag (negativo si el bit 63 es 1)
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; // Z flag (se activa si resultado es 0)
    printf("Updated X%d to 0x%" PRIx64 "\n", instruct.rd, NEXT_STATE.REGS[instruct.rd]);
}

void implement_SUBS_immediate(instruction instruct) {
    printf("Implementing SUBS(immediate)\n");

    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn];  // Valor del registro rn
    uint64_t op2 = instruct.alu_immediate;           // Inmediato ALU

    // Ejecutar la operación
    uint64_t result = op1 - op2;
    printf("op1: 0x%" PRIx64 ", op2: 0x%" PRIx64 ", result: 0x%" PRIx64 "\n", op1, op2, result);
    NEXT_STATE.REGS[instruct.rd] = result; // Guardar resultado en rd

    // Actualizar FLAGS (solo N y Z, porque C y V no están en CPU_State)
    NEXT_STATE.FLAG_N = (result >> 63) & 1; // N flag (negativo si el bit 63 es 1)
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; // Z flag (se activa si resultado es 0)
    printf("Updated X%d to 0x%" PRIx64 "\n", instruct.rd, NEXT_STATE.REGS[instruct.rd]);
}

void implement_SUBS_extended_register(instruction instruct) {
    printf("Implementing SUBS(Extended Register)\n");

    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn];  // Valor del registro rn
    uint64_t op2 = CURRENT_STATE.REGS[instruct.rm];  // Valor del registro rm

    // Ejecutar la operación
    uint64_t result = op1 - op2;
    printf("op1: 0x%" PRIx64 ", op2: 0x%" PRIx64 ", result: 0x%" PRIx64 "\n", op1, op2, result);
    NEXT_STATE.REGS[instruct.rd] = result; // Guardar resultado en rd

    // Actualizar FLAGS (solo N y Z, porque C y V no están en CPU_State)
    NEXT_STATE.FLAG_N = (result >> 63) & 1; // N flag (negativo si el bit 63 es 1)
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; // Z flag (se activa si resultado es 0)
    printf("Updated X%d to 0x%" PRIx64 "\n", instruct.rd, NEXT_STATE.REGS[instruct.rd]);
}

void implement_HLT(instruction instruct) {
    printf("Implementing HLT\n");
    RUN_BIT = 0; // Terminar la ejecución
}

void implement_ANDS_shifted_register(instruction instruct) {
    printf("Implementing ANDS(Shifted Register)\n");

    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn];  // Valor del registro rn
    uint64_t op2 = CURRENT_STATE.REGS[instruct.rm];  // Valor del registro rm

    // Ejecutar la operación
    uint64_t result = op1 & op2;
    printf("op1: 0x%" PRIx64 ", op2: 0x%" PRIx64 ", result: 0x%" PRIx64 "\n", op1, op2, result);
    NEXT_STATE.REGS[instruct.rd] = result; // Guardar resultado en rd

    // Actualizar FLAGS (solo N y Z, porque C y V no están en CPU_State)
    NEXT_STATE.FLAG_N = (result >> 63) & 1; // N flag (negativo si el bit 63 es 1)
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; // Z flag (se activa si resultado es 0)
    printf("Updated X%d to 0x%" PRIx64 "\n", instruct.rd, NEXT_STATE.REGS[instruct.rd]);
}

void implement_EOR_shifted_register(instruction instruct) {
    
    //   En el opcode se considerar que shift y N son siempre ceros, por lo que se chequean los bits
    //     <31:21>
    //     No se tiene que implementar el shift
    printf("Implementing EOR(Shifted Register)\n");

    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn];  // Valor del registro rn
    uint64_t op2 = CURRENT_STATE.REGS[instruct.rm];  // Valor del registro rm

    // Ejecutar la operación XOR
    uint64_t result = op1 ^ op2;
    printf("op1: 0x%" PRIx64 ", op2: 0x%" PRIx64 ", result: 0x%" PRIx64 "\n", op1, op2, result);

    NEXT_STATE.REGS[instruct.rd] = result; // Guardar resultado en rd

    // Actualizar FLAGS (solo N y Z)
    NEXT_STATE.FLAG_N = (result >> 63) & 1; // N flag (negativo si el bit 63 es 1)
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; // Z flag (se activa si resultado es 0)

    printf("Updated X%d to 0x%" PRIx64 "\n", instruct.rd, NEXT_STATE.REGS[instruct.rd]);
    // ACA EST EL ERROR DE QUE SE ACTIVA Z CUANDO ES CO PERO EN EL SIMULADOR DE REF NO PASA ESO!! ALGO CON EL SHIFT
}

void implement_MOVZ(instruction instruct) {
    printf("Implementing MOVZ\n");

    uint64_t imm = instruct.mov_immediate;  // Inmediato MOV

    // Ejecutar la operación (sin shift porque hw = 0)
    uint64_t result = imm;
    printf("imm: 0x%" PRIx64 ", result: 0x%" PRIx64 "\n", imm, result);
    NEXT_STATE.REGS[instruct.rd] = result; // Guardar resultado en rd

    // Actualizar FLAGS (solo N y Z, porque C y V no están en CPU_State)
    NEXT_STATE.FLAG_N = (result >> 63) & 1; // N flag (negativo si el bit 63 es 1)
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; // Z flag (se activa si resultado es 0)
    printf("Updated X%d to 0x%" PRIx64 "\n", instruct.rd, NEXT_STATE.REGS[instruct.rd]);
}

void implement_STURB(instruction instruct) {
    printf("Implementing STURB\n");

    uint64_t address = CURRENT_STATE.REGS[instruct.rn] + instruct.dt_address;  // Calculate the address
    uint8_t value = CURRENT_STATE.REGS[instruct.rd] & 0xFF;  // Get the byte value to store

    // Write the byte to memory (considering little-endian format)
    uint32_t aligned_address = address & ~0x3;  // Align the address to 4 bytes
    uint32_t aligned_value = mem_read_32(aligned_address);  // Read the aligned 32-bit value
    uint32_t byte_shift = (address & 0x3) * 8;  // Calculate the byte shift
    aligned_value = (aligned_value & ~(0xFF << byte_shift)) | (value << byte_shift);  // Insert the byte
    mem_write_32(aligned_address, aligned_value);  // Write the modified 32-bit value back to memory

    printf("Stored byte 0x%X at address 0x%" PRIx64 "\n", value, address);
}

void implement_LSL_immediate(instruction instruct) {
    printf("Implementing LSL(Immediate)\n");

    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn];  // Valor del registro rn
    uint64_t shift = instruct.shamt;                 // Shift amount

    // Ejecutar la operación
    uint64_t result = op1 << shift;
    printf("op1: 0x%" PRIx64 ", shift: 0x%" PRIx64 ", result: 0x%" PRIx64 "\n", op1, shift, result);
    NEXT_STATE.REGS[instruct.rd] = result; // Guardar resultado en rd

    // Actualizar FLAGS (solo N y Z, porque C y V no están en CPU_State)
    NEXT_STATE.FLAG_N = (result >> 63) & 1; // N flag (negativo si el bit 63 es 1)
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; // Z flag (se activa si resultado es 0)
    printf("Updated X%d to 0x%" PRIx64 "\n", instruct.rd, NEXT_STATE.REGS[instruct.rd]);
}




    // pag 211

