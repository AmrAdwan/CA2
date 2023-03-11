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
  /* TODO: write necessary fields in pipeline register */
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
  /* TODO: set instruction word on the instruction decoder */

  /* TODO: need a control signals class that generates control
   * signals from a given opcode and function code.
   */
  PC = if_id.PC;
  decoder.setInstructionWord(if_id.instruction);
  signals.setInstruction(decoder);
  if (decoder.getOpcode() != opcode::NOP)
      // decoder.getInstructionType() == InstructionType::typeR ||
      // decoder.getInstructionType() == InstructionType::typeS)
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

  /* TODO: register fetch and other matters */

  if (decoder.getOpcode() == opcode::JAL) 
  {
    regD = 9;
    regfile.setRD(regD);

  }
  if (decoder.getOpcode() == opcode::JR ) 
  {
    // regfile.setRS1( 9 );
    // id_ex.regB = 9;
    regfile.setRS2(9);

  }
  /* TODO: perhaps also determine and write the new PC here? */
}

void InstructionDecodeStage::clockPulse()
{
  /* ignore the "instruction" in the first cycle. */
  if (! pipelining || (pipelining && PC != 0x0))
    ++nInstrIssued;

  /* TODO: write necessary fields in pipeline register */
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
        NPC = PC + signals.add(decoder);
      }
      break;
    case opcode::BNF:
      if (!flag)
      {
        issued = 1;
        NPC = PC + signals.add(decoder);
      }
      break;
    case opcode::MACRC:
      if (decoder.getOpcode2() == opcode2::MOVHI)
      {
        if (((id_ex.immediate >> 15) & 0b1) == 0b1)
        {
          id_ex.immediate |= 0xffff0000;
        }
      }
      break;
    case opcode::JAL:
      issued = 1;
      NPC = PC + signals.add(decoder);
      linkReg = PC + 8;
      break;
    case opcode::JR:
      issued = 1;
      NPC = id_ex.regB;
      break;
    case opcode::J:
      issued = 1;
      NPC = PC + signals.add(decoder);
      break;
    case opcode::SFEQ:
      flag = (id_ex.regA == id_ex.regB);
      break;
    case opcode::SFLES:
      flag = (id_ex.regA <= id_ex.regB);
      break;
    case opcode::SFNE:
      flag = (id_ex.regA != id_ex.regB);
      break;
    case opcode::SFGES:
      flag = (id_ex.regA >= id_ex.regB);
      break;

    // case opcode::ADD:
    //   if (decoder.getOpcode2() == opcode2::SLL)
    //   {
    //     id_ex.regD = id_ex.regA << (id_ex.regB & 0b1111);
    //   }
    //   if (decoder.getOpcode2() == opcode2::EXTHZ)
    //   {
    //     id_ex.regB &= 0b1111;
    //     id_ex.regA >>= id_ex.regB;
    //     if (static_cast<int32_t>(id_ex.regA) & 0b10000)
    //     { 
    //       id_ex.regA |= 0xffffffe0;
    //       id_ex.regD = id_ex.regA;
    //     }
    //     else 
    //     {
    //       id_ex.regD = id_ex.regA;        
    //     }
    //   }
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
  id_ex.readSize = signals.getMemSize();
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
    alu.setOp(id_ex.action_ALU);
  }

  if (signals.getopcode() == opcode::MACRC)
  {
    if (signals.getopcode2() == opcode2::MOVHI)
    {
      alu.setA(immediate);
      alu.setB(16);
      alu.setOp(ALUOp::SLL);
    }
  }

  if (signals.getopcode() == opcode::ADD)
  {
    if (signals.getopcode2() == opcode2::SLL)
    {
      regB &= 0b1111;
      alu.setA(regA);
      alu.setB(regB);
      alu.setOp(ALUOp::SLL);
    }

    if (signals.getopcode2() == opcode2::EXTHZ)
    {
      regB &= 0b1111;
      alu.setA(regA);
      alu.setB(regB);
      alu.setOp(ALUOp::SRA);
    }
  }
  
}

void
ExecuteStage::clockPulse()
{
  /* TODO: write necessary fields in pipeline register. This
   * includes the result (output) of the ALU. For memory-operations
   * the ALU computes the effective memory address.
   */
  
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


  // if (signals.getopcode() == opcode::ADD)
  // {
  //   if (signals.getopcode2() == opcode2::EXTHZ)
  //   {
  //     if (ex_m.ALUout & 0b10000)
  //     {
  //       ex_m.ALUout |= 0xffffffe0;
  //       // std::cout << "aluouttt1111 = " << uint32_t(ex_m.ALUout) << '\n';
  //     }
  //   }
  // }

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
    dataMemory.setAddress(regD);
  }
  if (signals.getType() == InstructionType::typeS)
  {
    dataMemory.setWriteEnable(true);
    dataMemory.setAddress(ALUout);
    // std::cout << "readsize = " << int(ex_m.readSize) << '\n';
    dataMemory.setSize(ex_m.readSize);
    dataMemory.setDataIn(regB);
  }

  if (signals.getopcode() == opcode::LWZ || signals.getopcode() == opcode::LBS
      || signals.getopcode() == opcode::LBZ)
  {
    dataMemory.setAddress(ALUout);
    dataMemory.setSize(ex_m.readSize);
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

    // High-order bits of the loaded value are replaced
    // with bit 7 of the loaded value ( if it is 1 )
    if(signals.getopcode() == opcode::LBS &&
       static_cast<int>((m_wb.memRead >> 7) & 0b1) == 1)
    {
      m_wb.memRead = m_wb.memRead | 0xffffff00;
    }

    dataMemory.setReadEnable(false);
  } 
  if (actionMem == MemorySelector::store) 
  {
    // dataMemory.setDataIn(regB);
    // dataMemory.setWriteEnable(true); // to let the store instruction to write
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

  /* TODO: configure write lines of register file based on control
   * signals
   */
  actionWBOut = m_wb.actionWBOut;
  regfile.setRD(m_wb.regD);

  if (signals.getopcode() == opcode::LWZ ||
      signals.getopcode() == opcode::LBS ||
      signals.getopcode() == opcode::LBZ)
  {
    regfile.setWriteData(m_wb.memRead);
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
  /* TODO: pulse the register file */
  if (actionWBOut == WriteBackOutputSelector::write) 
  { 
    regfile.setWriteEnable(true);
    regfile.clockPulse();
    regfile.setWriteEnable(false);
  }
}
#pragma GCC diagnostic pop
