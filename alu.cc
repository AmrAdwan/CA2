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
    case ALUOp::JUMP:
      return A + (B << 2);
    case ALUOp::SUB:
      return A - B;
    case ALUOp::AND:
      return A & B;
    case ALUOp::OR:
      // std::cout << "OOOOOORRRRR = " << make(A | B) << '\n';
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
      // std::cout << "resultSRAAA = " << (A >> B) << '\n';
      return A >> B;
    case ALUOp::SLL:
      // std::cout << "resultslllll = " << (A << B) << '\n';
      return A << B;
    case ALUOp::NOP:
      return RegValue{0};

    /* TODO: implement necessary operations */

    default:
      throw IllegalInstruction("Unimplemented or unknown ALU operation");
  }

  return RegValue{0};
}
