/* rv64-emu -- Simple 64-bit RISC-V simulator
 *
 *    pipeline.cc - Classic 5-stage RISC pipeline
 *
 * Copyright (C) 2016-2021  Leiden University, The Netherlands.
 */

#include "pipeline.h"
#include "arch.h"


Pipeline::Pipeline(bool pipelining,
                   bool debugMode,
                   MemAddress &PC,
                   InstructionMemory &instructionMemory,
                   InstructionDecoder &decoder,
                   RegisterFile &regfile,
                   bool &flag,
                   MemAddress &NPC,
                   size_t &issued,
                   DataMemory &dataMemory)
  : pipelining{ pipelining }
{
  /* TODO: this might need modification in case the stages need access
   * to more shared components.
   */
  stages.emplace_back(std::make_unique<InstructionFetchStage>(pipelining,
                                                              if_id,
                                                              instructionMemory,
                                                              PC, NPC, issued));
  stages.emplace_back(std::make_unique<InstructionDecodeStage>(pipelining,
                                                               if_id, id_ex,
                                                               regfile,
                                                               decoder,
                                                               nInstrIssued,
                                                               nStalls,
                                                               flag,
                                                               NPC,
                                                               issued,
                                                               debugMode));
  stages.emplace_back(std::make_unique<ExecuteStage>(pipelining,
                                                     id_ex, ex_m));
  stages.emplace_back(std::make_unique<MemoryStage>(pipelining,
                                                    ex_m, m_wb,
                                                    dataMemory));
  stages.emplace_back(std::make_unique<WriteBackStage>(pipelining,
                                                       m_wb,
                                                       regfile, flag,
                                                       nInstrCompleted));
}

void
Pipeline::propagate()
{
  if (! pipelining)
    {
      /* Execute a single instruction execution step. */
      stages[currentStage]->propagate();
    }
  else
    {
      /* Run propagate for all stages within a single clock cycle. */
      for (auto &s : stages)
        s->propagate();
    }
}

void
Pipeline::clockPulse()
{
  if (! pipelining)
    {
      stages[currentStage]->clockPulse();
      currentStage = (currentStage + 1) % stages.size();
    }
  else
    {
      for (auto &s : stages)
        s->clockPulse();
    }
}
