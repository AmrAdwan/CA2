#ifndef CONTROLSIGNALS_H
#define CONTROLSIGNALS_H

#include "arch.h"
#include "inst-decoder.h"
#include "mux.h"

class ControlSignals 
{
  public:
    ControlSignals() = default;
    void setInstruction(const InstructionDecoder & decoder);
    uint32_t getInstruction();
    InstructionType getType();
    opcode getopcode();
    opcode2 getopcode2();
    uint32_t getimmediate();
    ALUOp getALUOp() const;
    InputSelectorA getSelectorALUInputA() const;
    InputSelectorB getSelectorALUInputB() const;
    MemorySelector getSelectorMemory() const;
    WriteBackInputSelector getSelectorWBInput() const;
    WriteBackOutputSelector getSelectorWBOutput() const;
    uint8_t getMemSize() const;
    bool getMemReadExtend() const;
    RegValue add(InstructionDecoder & decoder);

  private:
    opcode op;
    opcode2 op2;
    opcode3 op3;
    InstructionType type;
    InstructionDecoder decoder;
    int32_t immediate;
};
#endif // CONTROLSIGNALS_H