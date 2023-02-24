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
    // std::cout << "A " << A << " + " << "B " << B << '\n';
      return A + B;
    case ALUOp::JUMP:
      return A + (B << 2);
    case ALUOp::SUB:
      return A - B;
    case ALUOp::AND:
      return A & B;
    case ALUOp::OR:
      return  A | B;
    case ALUOp::EQ:
      if (A == B)
        return RegValue{1};
      return RegValue{0};
    case ALUOp::NEQ:
      if (A != B)
        return RegValue{1};
      return RegValue{0};
    case ALUOp::GE:
      if (A >= B)
        return RegValue{1};
      return RegValue{0};
    case ALUOp::LE:
      if (A <= B)
        return RegValue{1};
      return RegValue{0};
    case ALUOp::SRA:
      return A >> B;
    case ALUOp::SLL:
      B &= 0b1111;
      return A << B;
    case ALUOp::MOVHI:
      return B;
    case ALUOp::NOP:
      return RegValue{0};

    /* TODO: implement necessary operations */

    default:
      throw IllegalInstruction("Unimplemented or unknown ALU operation");
  }

  return RegValue{0};
}
