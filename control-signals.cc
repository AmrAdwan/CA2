#include "control-signals.h"
#include "alu.h"
#include "arch.h"
#include "inst-decoder.h"
#include "mux.h"
#include <iostream>


RegValue ControlSignals::add(InstructionDecoder & decoder)
{

  if (decoder.getOpcode() == opcode::J || decoder.getOpcode() == opcode::JAL)
  {
    return decoder.getImmediate() << 2;
  }
  else if (decoder.getOpcode() == opcode::JR || decoder.getOpcode() == opcode::JALR)
  {
    std::cout << "jrjrjrjrjrjrj" << '\n';
    return decoder.getB();
  }
  // return RegValue{0};
}

InstructionType ControlSignals::getType()
{
  return type;
}


opcode ControlSignals::getopcode ()
{
  return op;
}


uint32_t ControlSignals::getimmediate()
{
  return immediate;
}


ALUOp ControlSignals::getALUOp() const
{
  switch (op) 
  {
    case opcode::LBS:
    case opcode::LBZ:
    case opcode::LF:
    case opcode::LWA:
    case opcode::LWS:
    case opcode::LWZ:
    case opcode::ADDI:
    case opcode::SW:
    case opcode::SB:
      return ALUOp::ADD;


    // case opcode::JR:
    case opcode::JAL:
      return ALUOp::JUMP;

    
    case opcode::MACRC:
      if (op2 == opcode2::MOVHI) 
      {
        return ALUOp::MOVHI;
      }

    case opcode::ORI:
      return ALUOp::OR;

    case opcode::ADD:
      switch (op2) 
      {
        case opcode2::ADD:
          switch (op3) 
          {
            case opcode3::ADD:
              return ALUOp::ADD;
            case opcode3::SUB:
              return ALUOp::SUB;
            case opcode3::OR:
              return ALUOp::OR;
          }
          case opcode2::SLL:
            switch (op3) 
            {
              case opcode3::SLL:
                return ALUOp::SLL;
            }
          case opcode2::EXTHZ:
            switch (op3) 
            {
              case opcode3::SRA:
                return ALUOp::SRA;
            }
      }
    default:
      return ALUOp::NOP;
  }
}

InputSelectorA ControlSignals::getSelectorALUInputA() const
{
  switch (op) 
  {
    case opcode::ADDI:
    case opcode::LWZ:
    case opcode::SW:
    case opcode::SB:
    case opcode::ORI:
      return InputSelectorA::rs1;
    case opcode::ADD:
      switch (op2) 
      {
        case opcode2::ADD:
          switch (op3) 
          {
            case opcode3::ADD:
            case opcode3::SUB:
            case opcode3::OR:
              return InputSelectorA::rs1;
          }
        case opcode2::SLL:
        switch (op3) 
        {
          case opcode3::SLL:
            return InputSelectorA::rs1;
        }
        case opcode2::EXTHZ:
          switch (op3) 
          {
            case opcode3::SRA:
            return InputSelectorA::rs1;
          }
      }
    case opcode::MACRC:
      if (op2 == opcode2::MOVHI) 
      {
        return InputSelectorA::rd;
      }
  }
}

// get the second input of the ALU
InputSelectorB ControlSignals::getSelectorALUInputB() const
{
  switch (op) 
  {
    case opcode::ADD:
      switch (op2) 
      {
        case opcode2::ADD:
          switch (op3) 
          {
            case opcode3::ADD:
            case opcode3::SUB:
            case opcode3::OR:
              return InputSelectorB::rs2;
          } 
        case opcode2::SLL:
          switch (op3) 
          {
            case opcode3::SLL:
              return InputSelectorB::rs2;
          }
        case opcode2::EXTHZ:
          switch (op3) 
          {
            case opcode3::SRA:
              return InputSelectorB::rs2;
          }
      }
    case opcode::MACRC:
      if (op2 == opcode2::MOVHI) 
      {
        return InputSelectorB::immediate;
      }
    case opcode::JR:
      return InputSelectorB::rs2;
    case opcode::ADDI:
    case opcode::LBS:
    case opcode::LBZ:
    case opcode::LWA:
    case opcode::LWS:
    case opcode::LWZ:
    case opcode::SW:
    case opcode::SB:
    case opcode::J:
    case opcode::JAL:
    case opcode::ORI:
      return InputSelectorB::immediate;

    // default:
      // throw IllegalInstruction{"Unknown opcode enum value."};
      // return InputSelectorB::LAST;
  }
}

// determine if the instruction Load/Store or none
 MemorySelector ControlSignals::getSelectorMemory() const
 {
  switch (op) 
  {
    case opcode::LD:
    case opcode::LBS:
    case opcode::LBZ:
    case opcode::LF:
    case opcode::LWS:
    case opcode::LWZ:
      return MemorySelector::load;
    case opcode::SB:
    case opcode::SD:
    case opcode::SW:
      return MemorySelector::store;
    default:
      return MemorySelector::none;
    }
 }


WriteBackInputSelector ControlSignals::getSelectorWBInput() const
{
  switch (op) 
  {
    case opcode::LD:
    case opcode::LBS:
    case opcode::LBZ:
    case opcode::LF:
    case opcode::LWA:
    case opcode::LWS:
    case opcode::LWZ:
    case opcode::SW:
    case opcode::SB:
      return WriteBackInputSelector::memory;
    default:
      return WriteBackInputSelector::outputALU;
  }
}
        
        
WriteBackOutputSelector ControlSignals::getSelectorWBOutput() const
{
  switch (op) 
  {
    case opcode::LWS:
    case opcode::LWZ:
    case opcode::LWA:
    case opcode::ADD:
    case opcode::ADDI:
    case opcode::SW:
    case opcode::SB:
    case opcode::ORI:
    case opcode::MACRC:
      return WriteBackOutputSelector::write;

  }
  return WriteBackOutputSelector::none;
}


uint8_t ControlSignals::getMemSize() const 
{ 
  switch (op) 
  {
    case opcode::LWS:
    case opcode::LWZ:
    case opcode::SW:
      return 4u;
    case opcode::SB:
      return 1u;
    default:
      return 1u;
  }
}

bool ControlSignals::getMemReadExtend() const 
{
  return op == opcode::LWS || op == opcode::LWZ;
}

void ControlSignals::setInstruction(const InstructionDecoder & decoder)
{
  op = decoder.getOpcode();
  op2 = decoder.getOpcode2();
  op3 = decoder.getOpcode3();
  type = decoder.getInstructionType();
  immediate = decoder.getImmediate();
}