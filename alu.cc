/* rv64-emu -- Simple 64-bit RISC-V simulator
 *
 *    alu.h - ALU component.
 *
 * Copyright (C) 2016,2018  Leiden University, The Netherlands.
 */

#include "alu.h"
#include "arch.h"
#include "utils.h"
#include "inst-decoder.h"
#include <iostream>

#ifdef _MSC_VER
/* MSVC intrinsics */
#include <intrin.h>
#endif


ALU::ALU()
  : A(), B(), op()
{
}


RegValue
ALU::getResult()
{
 switch (op)
  {
    case ALUOp::ADD:
      return A + B;
    case ALUOp::SUB:
      return A - B;
    case ALUOp::AND:
      return A & B;
    case ALUOp::OR:
      return  A | B;
    case ALUOp::SRA:
      return A >> B;
    case ALUOp::SLL:
      return A << B;
    case ALUOp::NOP:
      return RegValue{0};
    default:
      throw IllegalInstruction("Unimplemented or unknown ALU operation");
  }

  return RegValue{0};
}
