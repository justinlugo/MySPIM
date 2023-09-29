#include "spimcore.h"

// Omar Ashry, Justin Lugo, Fabrizio Martins
// Prof. Angell
// CDA 3103 : Group 86
// 23 April 2021

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
  //Check ALUControl value and perform the desired operation
  switch(ALUControl)
  {
  case 0x0:
    *ALUresult = A + B;
    break;

  case 0x1:
    *ALUresult = A - B;
    break;

  case 0x2:
    if((unsigned)A < (unsigned)B)
    {
      *ALUresult = 1;
    }
    else
    {
      *ALUresult = 0;
    }
    break;

  case 0x3:
    if(A < B)
    {
      *ALUresult = 1;
    }
    else
    {
      *ALUresult = 0;
    }
    break;

  case 0x4:
    *ALUresult = A & B;
    break;

  case 0x5:
    *ALUresult = A | B;
    break;

  case 0x6:
    *ALUresult = B << 16;
    break;

  case 0x7:
    *ALUresult = !A;
    break;
  }

  //Set the proper zero value
  if(*ALUresult == 0)
  {
    *Zero = 1;
  }
  else
  {
    *Zero = 0;
  }

}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
  //fetch the instruction, if a halt conditon occurs, returns 1

  if(PC % 4 != 0)
  {
    return 1;
  }
  else
  {
    *instruction = Mem[PC >> 2];
  }

  return 0;
}

/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
  //function partitions the instruction to its desired bits
  *op = (instruction & 0xFC000000) >> 26; // 31 - 26
  *r1 = (instruction & 0x03F00000) >> 21; // 25 - 21
  *r2 =  (instruction & 0x001F0000) >> 16; // 20 - 16
  *r3 =  (instruction & 0x0000F800) >> 11; // 15 - 11
  *funct = (instruction & 0x0000003F); // 5 - 0
  *offset = (instruction & 0x0000FFFF); // 15 - 0
  *jsec =  (instruction & 0x03FFFFFF); // 25 - 0
}

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
  // The purpose of this function is to decode instructions from the opcode and
  // set the values of the control signals to their corresponding functions.

    //initial control signal values
    controls -> RegDst = 0;
    controls -> Jump = 0;
    controls -> Branch = 0;
    controls -> MemRead = 0;
    controls -> MemtoReg = 0;
    controls -> ALUOp = 0;
    controls -> MemWrite = 0;
    controls -> ALUSrc = 0;
    controls -> RegWrite = 0;

	switch(op)
	{
	    // R-Type (add, sub, and, or, slt, sltu), 0000 0000 -> 0.
        case 0x0:
            controls -> RegDst = 1;
            controls -> Jump = 0;
            controls -> Branch = 0;
            controls -> MemRead = 0;
            controls -> MemtoReg = 0;
            controls -> ALUOp = 7; // R-Type, 111 -> 7.
            controls -> MemWrite = 0;
            controls -> ALUSrc = 0;
            controls -> RegWrite = 1;
            break;
        // addi, 0000 1000 -> 8.
        case 0x8:
            controls -> RegDst = 0;
            controls -> Jump = 0;
            controls -> Branch = 0;
            controls -> MemRead = 0;
            controls -> MemtoReg = 0;
            controls -> ALUOp = 0; // Addition, 000 -> 0.
            controls -> MemWrite = 0;
            controls -> ALUSrc = 1;
            controls -> RegWrite = 1;
            break;
        // lw
        case 0x23:
            controls -> RegDst = 0;
            controls -> Jump = 0;
            controls -> Branch = 0;
            controls -> MemRead = 1;
            controls -> MemtoReg = 1;
            controls -> ALUOp = 0; // Load word function, 000 -> 0.
            controls -> MemWrite = 0;
            controls -> ALUSrc = 1;
            controls -> RegWrite = 1;
            break;
        // sw
        case 0x2b:
            controls -> RegDst = 0;
            controls -> Jump = 0;
            controls -> Branch = 0;
            controls -> MemRead = 0;
            controls -> MemtoReg = 0;
            controls -> ALUOp = 0; // Save word function, 000 -> 0.
            controls -> MemWrite = 1;
            controls -> ALUSrc = 1;
            controls -> RegWrite = 0;
            break;
        // lui
        case 0xf:
            controls -> RegDst = 0;
            controls -> Jump = 0;
            controls -> Branch = 0;
            controls -> MemRead = 0;
            controls -> MemtoReg = 0;
            controls -> ALUOp = 6; // Shift function, 110 -> 6.
            controls -> MemWrite = 0;
            controls -> ALUSrc = 1;
            controls -> RegWrite = 1;
            break;
        // slti
        case 0xa:
            controls -> RegDst = 0;
            controls -> Jump = 0;
            controls -> Branch = 0;
            controls -> MemRead = 0;
            controls -> MemtoReg = 0;
            controls -> ALUOp = 2; // Set less than function, 010 -> 2.
            controls -> MemWrite = 0;
            controls -> ALUSrc = 1;
            controls -> RegWrite = 1;
            break;
        // sltiu
        case 0xb:
            controls -> RegDst = 0;
            controls -> Jump = 0;
            controls -> Branch = 0;
            controls -> MemRead = 0;
            controls -> MemtoReg = 0;
            controls -> ALUOp = 3; // Set less than unsigned function, 011 -> 3.
            controls -> MemWrite = 0;
            controls -> ALUSrc = 1;
            controls -> RegWrite = 1;
            break;
        // beq
        case 0x4:
            controls -> RegDst = 0;
            controls -> Jump = 0;
            controls -> Branch = 1;
            controls -> MemRead = 0;
            controls -> MemtoReg = 0;
            controls -> ALUOp = 1; // Subtraction function, 001 -> 1.
            controls -> MemWrite = 0;
            controls -> ALUSrc = 0;
            controls -> RegWrite = 0;
            break;
        // jump
        case 0x2:
            controls -> RegDst = 0;
            controls -> Jump = 1;
            controls -> Branch = 0;
            controls -> MemRead = 0;
            controls -> MemtoReg = 0;
            controls -> ALUOp = 0; // Is a jump function, 000 -> 0.
            controls -> MemWrite = 0;
            controls -> ALUSrc = 0;
            controls -> RegWrite = 0;
            break;
        // If it doesn't suit any of the other cases.
        default:
            return 1;
	}
	// Otherwise:
	return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
  // The purpose of this function is to read the registers r1 and r2 and write the values
  // to data1 and data2.
  *data1 = Reg[r1];
  *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
  // The purpose of this function is to assign the sign-extended value of offset to
  // extended_value.
  if (offset >> 15 == 1)
  {
    // If the offset is negative, fill with 1's.
    *extended_value = (offset | 0xffff0000);
  }
  else
  {
    // Or else the offset is the extended value.
    *extended_value = offset;
  }
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
  //check for halt condition
  if(ALUOp < 0 || ALUOp > 7)
  {
    return 1;
  }

  unsigned tempd2;

  //check if we need to use extended value
  if(ALUSrc == 1)
  {
    tempd2 = extended_value;
  }
  else
  {
    tempd2 = data2;
  }

  //R-Type
  if(ALUOp == 7)
  {
    switch(funct)
    {
    case 0x20:
      ALUOp = 0x0;
      //ALU(data1, temp, 0x0, ALUresult, Zero);
      break;
    case 0x22:
      ALUOp = 0x1;
      //ALU(data1, temp, 0x1, ALUresult, Zero);
      break;
    case 0x2a:
      ALUOp = 0x2;
      //ALU(data1, temp, 0x2, ALUresult, Zero);
      break;
    case 0x2b:
      ALUOp = 0x3;
      //ALU(data1,temp, 0x3, ALUresult, Zero);
      break;
    case 0x24:
      ALUOp = 0x4;
      //ALU(data1, temp, 0x4, ALUresult, Zero);
      break;
    case 0x25:
      ALUOp = 0x5;
      //ALU(data1, temp, 0x5, ALUresult, Zero);
      break;
    default:
      return 1;
    }
    ALU(data1, tempd2, ALUOp, ALUresult, Zero);
  }
  else
  {
    //I types
    ALU(data1, tempd2, ALUOp, ALUresult, Zero);
  }

	return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
  //check if reading or writing memory, and assign the data as necessary
  if (MemRead == 1) {
		if ((ALUresult % 4) == 0) {
			*memdata = Mem[ALUresult >> 2];
		}else
			return 1;
	}

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
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
  //write the desired data to memory registers
  if(RegWrite == 1)
  {
    if(MemtoReg == 1)
    {
      Reg[r2] = memdata;
    }
    else if(MemtoReg == 0)
    {
      if(RegDst == 1)
      {
        Reg[r3] = ALUresult;
      }
      else
      {
        Reg[r2] = ALUresult;
      }
    }
  }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
  //updating the count
  *PC += 4;

	if (Zero == 1 && Branch == 1) {
    *PC += extended_value << 2;
	}
	if (Jump == 1) {
		*PC = (jsec << 2) | (*PC & 0xf0000000);
	}
}
