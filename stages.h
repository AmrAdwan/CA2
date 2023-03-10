/* rv64-emu -- Simple 64-bit RISC-V simulator
 *
 *    stages.h - Pipeline stages
 *
 * Copyright (C) 2016-2020  Leiden University, The Netherlands.
 */

#ifndef __STAGES_H__
#define __STAGES_H__

#include "alu.h"
#include "arch.h"
#include "mux.h"
#include "inst-decoder.h"
#include "memory-control.h"
#include "control-signals.h"
#include "utils.h"
#include <cstddef>



/* Pipeline registers may be read during propagate and may only be
 * written during clockPulse. Note that you cannot read the incoming
 * pipeline registers in clockPulse (e.g. in clockPulse of EX, you cannot
 * read ID_EX) because that register will already have been overwritten.
 * In case you need to propagate values from one pipeline register to
 * the next, these need to be buffered explicitly within the stage.
 */
struct IF_IDRegisters
{
  MemAddress     PC{0};
  MemAddress     NPC{0};
  instruction_t  instruction = 0;


  /* TODO: add necessary fields */
};

struct ID_EXRegisters
{
  MemAddress PC{0};
  MemAddress     NPC{0};
  RegValue   regA = 0;
  RegValue   regB = 0;
  RegValue   regD = 0;
  int32_t    immediate = 0;
  MemAddress linkReg{0};
  ControlSignals signals{};

  uint8_t    readSize = 8;
  WriteBackInputSelector   actionWBIn;
  WriteBackOutputSelector  actionWBOut = WriteBackOutputSelector::none;
  InputSelectorA           actionALUA;
  InputSelectorB           actionALUB;
  MemorySelector           actionMem = MemorySelector::none;
  ALUOp                    action_ALU = ALUOp::NOP;
  // BranchEnum            actionBranch = BranchEnum::none;
  bool                     memReadExtend = false;

  /* TODO: add necessary fields */
  Mux<MemAddress, PCSelector> mux;
};

struct EX_MRegisters
{
  MemAddress PC{0};
  MemAddress NPC{0};
  RegValue   regA = 0;
  RegValue   regB = 0;
  RegValue   regD = 0;
  int32_t    immediate = 0;
  MemAddress linkReg{0};
  ControlSignals signals{};
  
  
  MemAddress                branchPC = 0;
  RegValue                  ALUout = 0;
  uint8_t                   readSize = 8;
  WriteBackInputSelector    actionWBIn;
  WriteBackOutputSelector   actionWBOut = WriteBackOutputSelector::none;
  MemorySelector            actionMem = MemorySelector::none;
  PCSelector                actionPC = PCSelector::next;
  bool                      memReadExtend = false;

  
  /* TODO: add necessary fields */
};

struct M_WBRegisters
{
  MemAddress PC{0};
  MemAddress NPC{0};
  RegValue   regD = 0;
  RegValue   memRead{};
  RegValue   ALUout = 0;
  MemAddress linkReg{0};
  ControlSignals signals{};

  WriteBackInputSelector actionWBIn;
  WriteBackOutputSelector actionWBOut = WriteBackOutputSelector::none;


  /* TODO: add necessary fields */
};


/*
 * Abstract base class for pipeline stage
 */

class Stage
{
  public:
    Stage(bool pipelining)
      : pipelining(pipelining)
    { }

    virtual ~Stage()
    { }

    virtual void propagate() = 0;
    virtual void clockPulse() = 0;

  protected:
    bool pipelining;
};


/*
 * Instruction fetch
 */

class InstructionFetchFailure : public std::exception
{
  public:
    explicit InstructionFetchFailure(const MemAddress addr)
    {
      std::stringstream ss;
      ss << "Instruction fetch failed at address " << std::hex << addr;
      message = ss.str();
    }

    const char* what() const noexcept override
    {
      return message.c_str();
    }

  private:
    std::string message{};
};

class TestEndMarkerEncountered : public std::exception
{
  public:
    explicit TestEndMarkerEncountered(const MemAddress addr)
    {
      std::stringstream ss;
      ss << "Test end marker encountered at address " << std::hex << addr;
      message = ss.str();
    }

    const char* what() const noexcept override
    {
      return message.c_str();
    }

  private:
    std::string message{};
};


class InstructionFetchStage : public Stage
{
  public:
    InstructionFetchStage(bool pipelining,
                          IF_IDRegisters &if_id,
                          InstructionMemory instructionMemory,
                          MemAddress &PC, 
                          MemAddress &NPC,
                          size_t &issued)
      : Stage(pipelining),
      if_id(if_id),
      instructionMemory(instructionMemory),
      PC(PC),
      NPC(NPC),
      issued(issued)
    { }

    void propagate() override;
    void clockPulse() override;

  private:
    IF_IDRegisters &if_id;

    InstructionMemory instructionMemory;
    MemAddress &PC;
    MemAddress &NPC;
    size_t &issued;
    MemAddress linkReg{0};
    instruction_t instruction{0};
};

/*
 * Instruction decode
 */

class InstructionDecodeStage : public Stage
{
  public:
    InstructionDecodeStage(bool pipelining,
                           const IF_IDRegisters &if_id,
                           ID_EXRegisters &id_ex,
                           RegisterFile &regfile,
                           InstructionDecoder &decoder,
                           uint64_t &nInstrIssued,
                           uint64_t &nStalls,
                           bool &flag,
                           MemAddress &NPC,
                           size_t &issued,
                           bool debugMode = false)
      : Stage(pipelining),
      if_id(if_id), id_ex(id_ex),
      regfile(regfile), decoder(decoder),
      nInstrIssued(nInstrIssued), nStalls(nStalls),
      flag(flag), NPC(NPC), issued(issued), debugMode(debugMode)
    { }

    void propagate() override;
    void clockPulse() override;

  private:
    const IF_IDRegisters &if_id;
    ID_EXRegisters &id_ex;

    RegisterFile &regfile;
    InstructionDecoder &decoder;
    ControlSignals      signals{};
    uint64_t &nInstrIssued;
    uint64_t &nStalls;
    MemAddress linkReg{0};
    bool &flag;
    MemAddress &NPC;
    size_t &issued;


    bool debugMode = false;

    MemAddress PC{0};
    RegNumber regD{0};
};

/*
 * Execute
 */

class ExecuteStage : public Stage
{
  public:
    ExecuteStage(bool pipelining,
                 const ID_EXRegisters &id_ex,
                 EX_MRegisters &ex_m)
      : Stage(pipelining),
      id_ex(id_ex), ex_m(ex_m)
    { }

    void propagate() override;
    void clockPulse() override;

  private:
    const ID_EXRegisters &id_ex;
    EX_MRegisters &ex_m;
    RegValue   regA = 0;
    RegValue   regB = 0;
    RegValue   regD = 0;
    int32_t    immediate = 0;

    ALU alu{};
    bool memReadExtend = false;

    uint8_t readSize = 8;
    MemorySelector actionMem = MemorySelector::none;
    WriteBackInputSelector actionWBIn = WriteBackInputSelector::LAST;
    WriteBackOutputSelector actionWBOut = WriteBackOutputSelector::none;
    

    MemAddress PC{0};
    MemAddress linkReg{0};
    ControlSignals signals{};
    /* TODO: add other necessary fields/buffers and components (ALU anyone?) */
};

/*
 * Memory
 */

class MemoryStage : public Stage
{
  public:
    MemoryStage(bool pipelining,
                const EX_MRegisters &ex_m,
                M_WBRegisters &m_wb,
                DataMemory dataMemory)
      : Stage(pipelining),
      ex_m(ex_m), m_wb(m_wb), dataMemory(dataMemory)
    { }

    void propagate() override;
    void clockPulse() override;

  private:
    const EX_MRegisters &ex_m;
    M_WBRegisters &m_wb;

    DataMemory dataMemory;

    RegValue   regB = 0;
    RegValue   regD = 0;

    MemAddress PC{0};
    MemAddress linkReg{0};

    ControlSignals signals{};
    /* TODO: add other necessary fields/buffers */

    WriteBackInputSelector actionWBIn = WriteBackInputSelector::LAST;
    WriteBackOutputSelector actionWBOut = WriteBackOutputSelector::none;
    MemorySelector actionMem = MemorySelector::none;
    RegValue ALUout = 0;
    bool memReadExtend = false;
    Mux<MemAddress, PCSelector> muxPC{};
};

/*
 * Write back
 */

class WriteBackStage : public Stage
{
  public:
    WriteBackStage(bool pipelining,
                   const M_WBRegisters &m_wb,
                   RegisterFile &regfile,
                   bool &flag,
                   uint64_t &nInstrCompleted)
      : Stage(pipelining),
      m_wb(m_wb), regfile(regfile), flag(flag),
      nInstrCompleted(nInstrCompleted)
    { }

    void propagate() override;
    void clockPulse() override;

  private:
    const M_WBRegisters &m_wb;

    RegisterFile &regfile;
    bool &flag;
    MemAddress linkReg{0};
    ControlSignals signals{};

    /* TODO add other necessary fields/buffers and components */
    WriteBackOutputSelector actionWBOut = WriteBackOutputSelector::none;


    uint64_t &nInstrCompleted;
};


#endif /* __STAGES_H__ */
