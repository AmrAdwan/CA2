/* rv64-emu -- Simple 64-bit RISC-V simulator
 *
 *    stages.cc - Pipeline stages
 *
 * Copyright (C) 2016-2020  Leiden University, The Netherlands.
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"

#include "stages.h"
#include "alu.h"
#include "arch.h"
#include "control-signals.h"
#include "inst-decoder.h"
#include "memory-bus.h"
#include "memory-control.h"
#include "mux.h"
#include "reg-file.h"

#include <iostream>
#include <string.h>
#include <string>

/*
 * Instruction fetch
 */

void
InstructionFetchStage::propagate()
{
  try
  {
    if (issued != 2)
    {
      // instructionMemory.setAddress(PC);
      if (issued == 1) // this is needed for the dealy slot
      {
        issued++; 
      }

    }
    else if (issued == 2)
    {
      // to jump to the address that is given with the jump instruction
      PC = NPC;
      issued = 0;
      NPC = 0;
    }
    instructionMemory.setAddress(PC);
    instructionMemory.setSize(4);
    instruction = instructionMemory.getValue();

// #if 0
      /* Enable this once you have implemented instruction fetch. */
    if (instruction == TestEndMarker)
      throw TestEndMarkerEncountered(PC);

// #endif
  }
  catch (TestEndMarkerEncountered &e)
  {
    throw;
  }
  catch (std::exception &e)
  {
    throw InstructionFetchFailure(PC);
  }
}

void
InstructionFetchStage::clockPulse()
{
  if_id.PC = PC;
  if_id.instruction = instruction;
  PC += 4;
}

/*
 * Instruction decode
 */

void
dump_instruction(std::ostream &os, const uint32_t instructionWord,
                 const InstructionDecoder &decoder);

void
InstructionDecodeStage::propagate()
{
  PC = if_id.PC;
  decoder.setInstructionWord(if_id.instruction);
  signals.setInstruction(decoder);
  if (decoder.getOpcode() != opcode::NOP)
  {
    regfile.setRS1(decoder.getA()); // set the value of Register A
    regfile.setRS2(decoder.getB()); // set the value of Register B
    regD = decoder.getD();

    WriteBackOutputSelector shouldWrite = WriteBackOutputSelector::none;
    regfile.setRD(regD); // set the value of Register D
    regfile.setWriteData(RegValue{0});
    regfile.setWriteEnable(shouldWrite == WriteBackOutputSelector::write);
  }

  /* debug mode: dump decoded instructions to cerr.
   * In case of no pipelining: always dump.
   * In case of pipelining: special case, if the PC == 0x0 (so on the
   * first cycle), don't dump an instruction. This avoids dumping a
   * dummy instruction on the first cycle when ID is effectively running
   * uninitialized.
   */
  if (debugMode && (! pipelining || (pipelining && PC != 0x0)))
  {
    /* Dump program counter & decoded instruction in debug mode */
    auto storeFlags(std::cerr.flags());

    std::cerr << std::hex << std::showbase << PC << "\t";
    std::cerr.setf(storeFlags);

    std::cerr << decoder << std::endl;
  }

  // if instruction is JAL of JR then the destination register is 9
  if (decoder.getOpcode() == opcode::JAL) 
  {
    regD = 9;
    regfile.setRD(regD);

  }
  if (decoder.getOpcode() == opcode::JR ) 
  {
    regfile.setRS2(9);

  }
}

void InstructionDecodeStage::clockPulse()
{
  /* ignore the "instruction" in the first cycle. */
  if (! pipelining || (pipelining && PC != 0x0))
    ++nInstrIssued;

  id_ex.PC = PC;
  id_ex.signals = signals;
  id_ex.regA = regfile.getReadData1();
  id_ex.regB = regfile.getReadData2();
  id_ex.regD = decoder.getD();
  id_ex.immediate = decoder.getImmediate();

  switch (decoder.getOpcode()) 
  {
    case opcode::SB:
      id_ex.regB &= 0xff; // to get the lower 8-bits from Register B
      break;
    case opcode::SW:
      id_ex.regB &= 0xffffffff; // to get the 32-bits from register B
      break;
    case opcode::LWZ:
      id_ex.regD &= 0xffffffff; // to get the 32-bits from register D
      break;
    case opcode::LBS:
      id_ex.regD &= 0xff; // to get the lower 8-bits from Register D
      break;
    case opcode::LBZ:
      id_ex.regD &= 0xff; // to get the lower 8-bits from Register D
      break;
    case opcode::BF:
      if (flag)
      {
        issued = 1;
        NPC = PC + signals.add(decoder); // PC value added to the offset value
      }
      break;
    case opcode::BNF:
      if (!flag)
      {
        issued = 1;
        NPC = PC + signals.add(decoder); // PC value added to the offset value
      }
      break;
    case opcode::MACRC:
      if (decoder.getOpcode2() == opcode2::MOVHI)
      {
        if (((id_ex.immediate >> 15) & 0b1) == 0b1) // if the highest bit 1 is then sign extend
        {
          id_ex.immediate |= 0xffff0000; // Sign extenstion 
        }
      }
      break;
    case opcode::JAL:
      issued = 1;
      NPC = PC + signals.add(decoder); // PC value added to the offset value
      linkReg = PC + 8; // PC + 8 for dealy slots
      break;
    case opcode::JR:
      issued = 1;
      NPC = id_ex.regB; // PC value get the value of register B
      break;
    case opcode::J:
      issued = 1;
      NPC = PC + signals.add(decoder); // PC value added to the offset value
      break;
    case opcode::SFEQ:
      flag = (id_ex.regA == id_ex.regB); // flag is true if register A and register B are equal
      break;
    case opcode::SFLES:
      flag = (id_ex.regA <= id_ex.regB); // flag is true if register A less than or equal to register B
      break;
    case opcode::SFNE:
      flag = (id_ex.regA != id_ex.regB); // flag is true if register A and register B are NOT equal
      break;
    case opcode::SFGES:
      flag = (id_ex.regA >= id_ex.regB); // flag is true if register A greater than or equal to register B
      break;
  }

  id_ex.PC = PC;
  id_ex.linkReg = linkReg;
  id_ex.regD = regD;
  id_ex.signals = signals;
  id_ex.actionALUA = signals.getSelectorALUInputA(); // get the first input of the ALU
  id_ex.actionALUB = signals.getSelectorALUInputB(); // get the second input of the ALU
  id_ex.actionMem = signals.getSelectorMemory(); // get if the instruction is a memory instruction
  id_ex.actionWBOut = signals.getSelectorWBOutput();
  id_ex.action_ALU = signals.getALUOp(); // get the ALU operation
  id_ex.actionWBIn = signals.getSelectorWBInput();
  id_ex.memReadExtend = signals.getMemReadExtend();
  id_ex.readSize = signals.getMemSize(); // get the memory size for load/store instructions
}

/*
 * Execute
 */

void
ExecuteStage::propagate()
{
  PC = id_ex.PC;
  linkReg = id_ex.linkReg;
  signals = id_ex.signals;
  regA = id_ex.regA;
  regB = id_ex.regB;
  regD = id_ex.regD;
  actionMem = id_ex.actionMem;
  actionWBIn = id_ex.actionWBIn;
  actionWBOut = id_ex.actionWBOut;
  memReadExtend = id_ex.memReadExtend;
  readSize = id_ex.readSize;
  immediate = id_ex.immediate;

  if (signals.getopcode() != opcode::BF && signals.getopcode() != opcode::JR &&
      signals.getopcode() != opcode::J && signals.getopcode() != opcode::JAL &&
      signals.getopcode() != opcode::JALR && signals.getopcode() != opcode::BNF &&
      signals.getopcode() != opcode::NOP && signals.getopcode() != opcode::SFNE && 
      signals.getopcode() != opcode::SFEQ && signals.getopcode() != opcode::SFLES && 
      signals.getopcode() != opcode::SFGES && signals.getopcode() != opcode::MACRC)
  {
    { // Set input A.
        Mux<RegValue, InputSelectorA> mux;
        mux.setInput(InputSelectorA::rs1, regA);
        mux.setInput(InputSelectorA::rd, regD);
        mux.setSelector(id_ex.actionALUA);
        alu.setA(mux.getOutput());
    }
    
    { // Set input B.
        Mux<RegValue, InputSelectorB> mux;
        mux.setInput(InputSelectorB::rs2, id_ex.regB);
        mux.setInput(InputSelectorB::immediate, id_ex.immediate);
        mux.setSelector(id_ex.actionALUB);
        alu.setB(mux.getOutput());

    }
    // set ALU operation beased on the above
    alu.setOp(id_ex.action_ALU);
  }
  // set the ALU inputs if the instruction is MOVHI
  if (signals.getopcode() == opcode::MACRC)
  {
    if (signals.getopcode2() == opcode2::MOVHI)
    {
      alu.setA(immediate);
      alu.setB(16); // to shift the immediate with 16 bits
      alu.setOp(ALUOp::SLL); // Set the ALU operation as shift left logic
    }
  }

  if (signals.getopcode() == opcode::ADD)
  {
    if (signals.getopcode2() == opcode2::SLL)
    {
      regB &= 0b1111; // to ignore the fifth bit of register B
      alu.setA(regA);
      alu.setB(regB);
      alu.setOp(ALUOp::SLL); // Set the ALU operation as shift left logic
    }

    if (signals.getopcode2() == opcode2::EXTHZ)
    {
      regB &= 0b1111; // to ignore the fifth bit of register B
      alu.setA(regA);
      alu.setB(regB);
      alu.setOp(ALUOp::SRA); // Set the ALU operation as shift right arithmetic
    }
  }
  
}

void
ExecuteStage::clockPulse()
{
  if (signals.getopcode() != opcode::BF && signals.getopcode() != opcode::JR &&
      signals.getopcode() != opcode::J &&  signals.getopcode() != opcode::JALR && 
      signals.getopcode() != opcode::BNF && signals.getopcode() != opcode::NOP &&
      signals.getopcode() != opcode::SFNE && signals.getopcode() != opcode::SFEQ &&
      signals.getopcode() != opcode::SFLES && signals.getopcode() != opcode::SFGES)
  {
    ex_m.ALUout = alu.getResult();
  }

  if (signals.getopcode() == opcode::JAL)
  {
    ex_m.ALUout = linkReg;
  } 

  ex_m.PC = PC;
  ex_m.actionMem = actionMem;
  ex_m.actionWBIn = actionWBIn;
  ex_m.actionWBOut = actionWBOut;
  ex_m.linkReg = linkReg;
  ex_m.signals = signals;
  ex_m.regD = regD;
  ex_m.regA = regA;
  ex_m.regB = regB;
  ex_m.memReadExtend = memReadExtend;
  ex_m.readSize = readSize;
  ex_m.immediate = immediate;
}

/*
 * Memory
 */

void
MemoryStage::propagate()
{
  PC = ex_m.PC;
  actionWBIn = ex_m.actionWBIn;
  actionWBOut = ex_m.actionWBOut;
  linkReg = ex_m.linkReg;
  signals = ex_m.signals;
  ALUout = ex_m.ALUout;
  regB = ex_m.regB;
  regD = ex_m.regD;
  actionMem = ex_m.actionMem;
  memReadExtend = ex_m.memReadExtend; 
  dataMemory.setSize(ex_m.readSize);

  if (signals.getopcode() == opcode::JAL)
  {
    dataMemory.setAddress(regD); // memory address is register D
  }
  if (signals.getType() == InstructionType::typeS)
  {
    dataMemory.setWriteEnable(true); // enable writing
    dataMemory.setAddress(ALUout); // address is the result of the ALU
    dataMemory.setSize(ex_m.readSize); // set size with the suitable size based on instruction
    dataMemory.setDataIn(regB);
  }

  if (signals.getopcode() == opcode::LWZ || signals.getopcode() == opcode::LBS
      || signals.getopcode() == opcode::LBZ)
  {
    dataMemory.setAddress(ALUout); // address is the result of the ALU
    dataMemory.setSize(ex_m.readSize); // set size with the suitable size based on instruction
  }
}

void
MemoryStage::clockPulse()
{
  if (actionMem == MemorySelector::load) 
  {
    dataMemory.setReadEnable(true); // to let the load instruction to read
    dataMemory.setDataIn(regD);
    m_wb.memRead = dataMemory.getDataOut(memReadExtend);

    /* High-order bits of the loaded value are replaced
       with bit 7 of the loaded value ( if it is 1 ) */
    if(signals.getopcode() == opcode::LBS &&
       static_cast<int>((m_wb.memRead >> 7) & 0b1) == 1)
    {
      m_wb.memRead = m_wb.memRead | 0xffffff00;
    }

    dataMemory.setReadEnable(false);
  } 
  if (actionMem == MemorySelector::store) 
  {
    dataMemory.clockPulse();
    dataMemory.setWriteEnable(false);
  }
  m_wb.PC = PC;
  m_wb.actionWBIn = actionWBIn;
  m_wb.actionWBOut = actionWBOut;
  m_wb.regD = regD;
  m_wb.ALUout = ALUout;
  m_wb.linkReg = linkReg;
  m_wb.signals = signals;
}

/*
 * Write back
 */

void
WriteBackStage::propagate()
{
  if (! pipelining || (pipelining && m_wb.PC != 0x0))
    ++nInstrCompleted;
  signals = m_wb.signals;
  linkReg = m_wb.linkReg;
  actionWBOut = m_wb.actionWBOut;
  regfile.setRD(m_wb.regD);

  if (signals.getopcode() == opcode::LWZ ||
      signals.getopcode() == opcode::LBS ||
      signals.getopcode() == opcode::LBZ)
  {
    regfile.setWriteData(m_wb.memRead); // wrtie based on suitable size
  }

  if (signals.getopcode() != opcode::NOP && signals.getType() != InstructionType::typeS)
  {
    Mux<RegValue, WriteBackInputSelector> mux;
    mux.setInput(WriteBackInputSelector::memory, m_wb.memRead);
    mux.setInput(WriteBackInputSelector::outputALU, m_wb.ALUout);
    mux.setSelector(m_wb.actionWBIn);
    regfile.setWriteData(mux.getOutput());
  }
  if (signals.getopcode() == opcode::JAL)
  {
    regfile.setRD(m_wb.regD);
    regfile.setWriteData(linkReg);
  }
}

void
WriteBackStage::clockPulse()
{
  if (actionWBOut == WriteBackOutputSelector::write) 
  { 
    regfile.setWriteEnable(true);
    regfile.clockPulse();
    regfile.setWriteEnable(false);
  }
}
#pragma GCC diagnostic pop
