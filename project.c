/*
CDA FINAL PROJECT
Rayyan Jamil and Muhammad Khan
3103C-23
*/

#include "spimcore.h"
#include <stdio.h>

/* ALU */
/* 10 Points */
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult,
         char *Zero) {
  switch ((int)ALUControl) {
  // Add
  case 0:
    *ALUresult = A + B;
    break;
  // Subtract
  case 1:
    *ALUresult = A - B;
    break;
  // Set on less than (signed)
  case 2:
    if ((signed)A < (signed)B)
      *ALUresult = 1;
    else
      *ALUresult = 0;
    break;
    // Set on less than (unsigned)
  case 3:
    if (A < B)
      *ALUresult = 1;
    else
      *ALUresult = 0;
    break;
  // AND
  case 4:
    *ALUresult = A & B;
    break;
  // OR
  case 5:
    *ALUresult = A | B;
    break;
  // Shift left B by 16 bits
  case 6:
    *ALUresult = B << 16;
    break;
  // NOT A
  case 7:
    *ALUresult = ~A;
    break;
  }

  // SET zero
  if (*ALUresult == 0)
    *Zero = 1;
  else
    *Zero = 0;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction) {
  // Multiply by 4 to get the proper address in memory
  unsigned index = PC >> 2;

  // PC is word compatible
  if (PC % 4 != 0)
    return 1;
  // Set intruction equal to memory address
  *instruction = Mem[index];
  return 0;
}

/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,
                           unsigned *r2, unsigned *r3, unsigned *funct,
                           unsigned *offset, unsigned *jsec) {
  // Take fields from intruction
  *op = (instruction >> 26) &
        0b00000000000000000000000000111111; // instruction[31-26]
  *r1 = (instruction >> 21) & 0b11111;      // instruction[25-21]
  *r2 = (instruction >> 16) & 0b11111;      // instruction[20-16]
  *r3 = (instruction >> 11) & 0b11111;      // instruction[15-11]
  *funct = instruction & 0b00000000000000000000000000111111; // instruction[5-0]
  *offset =
      instruction & 0b00000000000000001111111111111111;     // instruction[15-0]
  *jsec = instruction & 0b00000011111111111111111111111111; // instruction[25-0]
}

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op, struct_controls *controls) {
  // The values we set depend on which opcode is recieved
  switch (op) {

    // R Type Instructions

  case 0:
    controls->RegDst = 1;
    controls->RegWrite = 1;
    controls->ALUSrc = 0;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->MemtoReg = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->ALUOp = 7;
    break;

  // I Type Instructions

  // addi
  case 8:
    controls->RegDst = 0;
    controls->RegWrite = 1;
    controls->ALUSrc = 1;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->MemtoReg = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->ALUOp = 0;
    break;

  // slt
  case 10:
    controls->RegDst = 1;
    controls->RegWrite = 1;
    controls->ALUSrc = 0;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->MemtoReg = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->ALUOp = 2;
    break;

  // sltu
  case 11:
    controls->RegDst = 1;
    controls->RegWrite = 1;
    controls->ALUSrc = 0;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->MemtoReg = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->ALUOp = 3;
    break;

  // beq
  case 4:
    controls->RegDst = 2;
    controls->RegWrite = 0;
    controls->ALUSrc = 0;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->MemtoReg = 2;
    controls->Jump = 0;
    controls->Branch = 1;
    controls->ALUOp = 1;
    break;

  // lw
  case 35:
    controls->RegDst = 0;
    controls->RegWrite = 1;
    controls->ALUSrc = 1;
    controls->MemRead = 1;
    controls->MemWrite = 0;
    controls->MemtoReg = 1;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->ALUOp = 0;
    break;

  // lui
  case 15:
    controls->RegDst = 0;
    controls->RegWrite = 1;
    controls->ALUSrc = 1;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->MemtoReg = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->ALUOp = 6;
    break;

  // sw
  case 43:
    controls->RegDst = 2;
    controls->RegWrite = 0;
    controls->ALUSrc = 1;
    controls->MemRead = 0;
    controls->MemWrite = 1;
    controls->MemtoReg = 2;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->ALUOp = 0;
    break;

  // J Type Instructions

  // Jump
  case 2:
    controls->RegDst = 0;
    controls->RegWrite = 0;
    controls->ALUSrc = 0;
    controls->MemRead = 0;
    controls->MemWrite = 0;
    controls->MemtoReg = 0;
    controls->Jump = 1;
    controls->Branch = 0;
    controls->ALUOp = 0;
    break;

  default:
    return 1; // Unknown opcode
  }

  return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1, unsigned r2, unsigned *Reg, unsigned *data1,
                   unsigned *data2) {
  // Fill data1 and data2 with the values from register array
  *data1 = Reg[r1];
  *data2 = Reg[r2];
}

/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset, unsigned *extended_value) {
  unsigned negative = offset >> 15;
  // If the number is negative (MSB = 1), extend with 1s
  if (negative == 1)
    *extended_value = offset | 0xFFFF0000;
  // Otherwise extend with 0's
  else
    *extended_value = offset & 0x0000FFFF;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value,
                   unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult,
                   char *Zero) {
  // If the ALUSrc is 1, set data2 to extended_value
  if (ALUSrc == 1) {
    data2 = extended_value;
  }

  // 7 is R-type instruction
  if (ALUOp == 7) {
    // Set the correct ALUOp for each R-type instruction
    switch (funct) {
    // Add
    case 32:
      ALUOp = 0;
      break;
    // Subtract
    case 34:
      ALUOp = 1;
      break;
    // Set on less than (signed)
    case 42:
      ALUOp = 2;
      break;
    // Set on less than (unsigned)
    case 43:
      ALUOp = 3;
      break;
    // AND
    case 36:
      ALUOp = 4;
      break;
    // OR
    case 37:
      ALUOp = 5;
      break;
    // Shift left right variable
    case 4:
      ALUOp = 6;
      break;
    // NOR
    case 39:
      ALUOp = 7;
      break;
    // Default for halt or don't care
    default:
      return 1;
    }
    // Call ALU function
    ALU(data1, data2, ALUOp, ALUresult, Zero);
  } else {
    // Call ALU for non-functions
    ALU(data1, data2, ALUOp, ALUresult, Zero);
  }

  return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead,
              unsigned *memdata, unsigned *Mem) {
  // If we are reading,
  // Read data from ALUresult * 4 index in Memory.
  if (MemRead == 1) {
    if ((ALUresult % 4) == 0) {
      *memdata = Mem[ALUresult >> 2];
    } else
      return 1;
  }

  // If we are writing,
  // Write data to ALUresult * 4 index in Memory
  // Word aligned
  if (MemWrite == 1) {
    if ((ALUresult % 4) == 0) {
      Mem[ALUresult >> 2] = data2;
    } else
      return 1;
  }

  return 0;
}

/* Write Register */
/* 10 Points */
void write_register(unsigned r2, unsigned r3, unsigned memdata,
                    unsigned ALUresult, char RegWrite, char RegDst,
                    char MemtoReg, unsigned *Reg) {
  // If we're writing
  if (RegWrite == 1) {
    // Memory to register
    if (MemtoReg == 1 && RegDst == 0)
      Reg[r2] = memdata;
    // Memory to register but to r3
    else if (MemtoReg == 1 && RegDst == 1)
      Reg[r3] = memdata;
    // Store ALUResult in r2
    else if (MemtoReg == 0 && RegDst == 0)
      Reg[r2] = ALUresult;
    // Store ALUResult in r3
    else if (MemtoReg == 0 && RegDst == 1)
      Reg[r3] = ALUresult;
  }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump,
               char Zero, unsigned *PC) {
  // Always increment by 4 no matter what.
  *PC += 4;

  // If branching when there is a zero, add the extended_value, bitshifted
  //   (multiplied by 4)
  if (Zero == 1 && Branch == 1)
    *PC += extended_value << 2;

  // If jumping, jump to the Jump register and multiply by 4 (word align)
  if (Jump == 1)
    *PC = (jsec << 2) | (*PC & 0xf0000000);
}
