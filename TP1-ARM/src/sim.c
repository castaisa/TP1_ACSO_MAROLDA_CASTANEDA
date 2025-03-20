#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "shell.h"

#define OPCODE_TABLE_SIZE 8
// #define MASK_21 0x7FF
// #define MASK_26 0x3F
// #define MASK_24 0x3F
// #define MASK_22 0x7FF
#define MASK_21 0b00000111111111111  // 0x7FF  = 0000 0111 1111 1111 (11 bits en 1)
#define MASK_26 0b00111111          // 0x3F   = 0011 1111 (6 bits en 1)
#define MASK_24 0b00111111          // 0x3F   = 0011 1111 (6 bits en 1)
#define MASK_22 0b00000111111111111 // 0x7FF  = 0000 0111 1111 1111 (11 bits en 1)

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
    char type[4];  // Ahora puede almacenar "R", "I", "D", "B", "CB", "IW"
    char *name;     // Nombre de la instrucción
} instruction;

void process_instruction();
instruction decode_instruction(uint32_t bytecode);
void decode_instruction_opcode(instruction *instr, uint32_t bytecode);
void decode_completely_instruction(instruction *instr, uint32_t bytecode);
void implement_ADDS(instruction instruct);

// void decode_instruction_with_opcode(instruction *instr);


instruction opcode_table[OPCODE_TABLE_SIZE] = {
    // ADD/SUBSTRACT (immediate)
    {000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G1", "ADDS(Extended Register)"},
    {10110001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G1", "ADDS(immediate)"},
    {000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G1", "SUBS(Extended Register)"},
    {11110001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G1", "SUBS(immediate)"},
    {000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "HLT"},
    {000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "CMP"},
    {1111001000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G3", "ANDS(immediate)"},
    {1101001000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G3", "EOR(immediate)"},
    {1011001000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G3", "ORR(immediate)"},
    {000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "B"},
    {000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "BR"},
    {000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "BEQ"},
    {000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "BNE"},
    {000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "BGT"},
    {000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "BLT"},
    {000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "BGE"},
    {000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "BLE"},
    {11010011011, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "LSL"},
    {11010011010, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "LSR"},
    {11111000000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "STUR"},
    {11111000010, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "STURB"},
    // {00111000000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "STURB"},   // pg 235
    {11111000001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "STURH"},
    // {01111000000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "STURH"},   // pg 235
    {11111000000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "LDUR"},
    // {11111000001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "LDUR"},    // pg 235
    {11111000010, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "LDURH"},
    // {01111000001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "LDURH"},   // pg 235
    {11111000001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "LDURB"},
    // {00111000001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "LDURB"},   // pg 235

    {110100101, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "MOVZ"}
    {000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "G1", "ADD(Extended Register)"},
    {10010001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "ADD(immediate)"},
    {000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "MUL"},
    {10110100, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "CBZ"}
    {10110101, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "CBNZ"}

    // {11010001, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "SUB(immediate)"},
    // {1001001000, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", "AND(immediate)"},

};


void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. 
     * */

    uint32_t bytecode = mem_read_32(CURRENT_STATE.PC);
    instruction instruct = decode_instruction(bytecode);

    // falta:
        // funcion que con el opcod complete el resto de la instruccion
        // tabla de hash con los posibles opcodes
    if (instruct.name == "ADDS(immediate)") implement_ADDS(instruct);
    // if (instruct.name == "SUBS") implement_SUBS(instruct);
    // if (instruct.name == "HLT") implement_HLT(instruct);
    // if (instruct.name == "CMP") implement_CMP(instruct);
    // if (instruct.name == "ANDS") implement_ANDS(instruct);
    // if (instruct.name == "EOR") implement_EOR(instruct);
    // if (instruct.name == "ORR") implement_ORR(instruct);
    // if (instruct.name == "B") implement_B(instruct);
    // if (instruct.name == "BR") implement_BR(instruct);
    // if (instruct.name == "BEQ") implement_BEQ(instruct);
    // if (instruct.name == "BNE") implement_BNE(instruct);
    // if (instruct.name == "BGT") implement_BGT(instruct);
    // if (instruct.name == "BLT") implement_BLT(instruct);
    // if (instruct.name == "BGE") implement_BGE(instruct);
    // if (instruct.name == "BLE") implement_BLE(instruct);
    // if (instruct.name == "LSL") implement_LSL(instruct);
    // if (instruct.name == "LSR") implement_LSR(instruct);
    // if (instruct.name == "STUR") implement_STUR(instruct);
    // if (instruct.name == "STURB") implement_STURB(instruct);
    // if (instruct.name == "STURH") implement_STURH(instruct);
    // if (instruct.name == "LDUR") implement_LDUR(instruct);
    // if (instruct.name == "LDURB") implement_LDURB(instruct);
    // if (instruct.name == "LDURH") implement_LDURH(instruct);
    // if (instruct.name == "MOVZ") implement_MOVZ(instruct);
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
    
    // type R, D, IW
    uint32_t opcode_21 = (bytecode >> 21) & MASK_21;

    for (int i = 0; i < OPCODE_TABLE_SIZE; i++) {
        if (opcode_table[i].opcode == opcode_21) {
            instr->opcode = opcode_table[i].opcode;
            instr->name = opcode_table[i].name;
            strcpy(instr->type, opcode_table[i].type);
            return;
        }
    }

    // type B
    uint32_t opcode_26 = (bytecode >> 26) & MASK_26;
    for (int i = 0; i < OPCODE_TABLE_SIZE; i++) {
        if (opcode_table[i].opcode == opcode_26) {
            instr->opcode = opcode_table[i].opcode;
            instr->name = opcode_table[i].name;
            strcpy(instr->type, opcode_table[i].type);
            return;
        }
    }

    // type CB
    uint32_t opcode_24 = (bytecode >> 24) & MASK_24;
    for (int i = 0; i < OPCODE_TABLE_SIZE; i++) {
        if (opcode_table[i].opcode == opcode_24) {
            instr->opcode = opcode_table[i].opcode;
            instr->name = opcode_table[i].name;
            strcpy(instr->type, opcode_table[i].type);
            return;
        }
    }

    // type I
    uint32_t opcode_22 = (bytecode >> 22) & MASK_22;
    for (int i = 0; i < OPCODE_TABLE_SIZE; i++) {
        if (opcode_table[i].opcode == opcode_22) {
            instr->opcode = opcode_table[i].opcode;
            instr->name = opcode_table[i].name;
            strcpy(instr->type, opcode_table[i].type);
            return;
        }
    }    
}

void decode_completely_instruction(instruction *instr, uint32_t bytecode) {
    // falta completar el resto de la instruccion con el opcode
    // if instr->name == 'R':
    // if instr->name == 'I':
    // if instr->name == 'D':
    // if instr->name == 'B':
    // if instr->name == 'CB':
    // if instr->name == 'IW':
    
    // instr->rd = 
    // instr->rn = 
    // instr->rm = 
    // instr->shamt = 
    // instr->alu_immediate = 
    // instr->dt_address = 
    // instr->br_address = 
    // instr->cond_br_address = 
    // instr->mov_immediate = 
    // instr->type = 
}

// INSTRUCCIONES -------------------------------------------------------------------------------------------

void implement_ADDS(instruction instruct) {
    uint64_t op1 = CURRENT_STATE.REGS[instruct.rn];  // Valor del registro rn
    uint64_t op2;

    // Decodificar el segundo operando
    if (strcmp(instruct.type, "I") == 0) { // Tipo inmediato
        op2 = instruct.alu_immediate;
        if (instruct.shamt == 1) {  // Aplicar shift si es necesario
            op2 <<= 12;
        }
    } else { // Tipo registro
        op2 = CURRENT_STATE.REGS[instruct.rm];
    }

    // Ejecutar la operación
    uint64_t result = op1 + op2;
    NEXT_STATE.REGS[instruct.rd] = result; // Guardar resultado en rd

    // Actualizar FLAGS (solo N y Z, porque C y V no están en CPU_State)
    NEXT_STATE.FLAG_N = (result >> 63) & 1; // N flag (negativo si el bit 63 es 1)
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0; // Z flag (se activa si resultado es 0)
}






// EN EL MANUAL MIRAR A64
    // pag 211
