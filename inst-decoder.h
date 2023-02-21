/* rv64-emu -- Simple 64-bit RISC-V simulator
 *
 *    inst-decoder.h - RISC-V instruction decoder.
 *
 * Copyright (C) 2016,2019  Leiden University, The Netherlands.
 */

#ifndef __INST_DECODER_H__
#define __INST_DECODER_H__

#include "arch.h"
#include "reg-file.h"

#include <stdexcept>
#include <cstdint>

static const int INSTRUCTION_SIZE = 4;

static constexpr uint32_t NOPMarker   = 0x0;
static constexpr uint32_t STALLMarker = 0x1;

/* TODO: add enums and constants necessary for your instruction decoder. */

enum class opcode 
{
  // typeR
  ADD, // 0x38 Add (opcode2 0x0) (opcode3 0x0) -> typeR
      // ADDC -> Add and Carry (opcode2 0x0) (opcode3 0x1) -> typeR
      // AND -> And (opcode2 0x0) (opcode3 0x3) -> typeR
      // CMOV -> Conditional Move (opcode2 0x0) (opcode3 0xe) -> typeR
      // DIV -> Divide Signed (opcode2 0x3) (opcode3 0x9) -> typeR
      // DIVU -> Divide Unsigned (opcode2 0x3) (opcode3 0xa) -> typeR
      // MUL -> Multiply Signed (opcode2 0x3) (opcode3 0x6) -> typeR
      // MULU -> Multiply Unsigned (opcode2 0x3) (opcode3 0xb) -> typeR
      // MULD -> Multiply Signed to Doubl (opcode2 0x3) (opcode3 0x7) -> NOTYPE
      // MULDU -> Multiply Unsigned to Double (opcode2 0x3) (opcode3 0xc) -> NOTYPE
      // OR -> Or (opcode2 0x0) (opcode3 0x4) -> typeR
      // ROR -> Rotate Right (opcode2 0x3) (opcode3 0x8) -> NOTYPE
      // SUB -> Subtract (opcode2 0x0) (opcode3 0x2) -> typeR
      // XOR -> Exclusive Or (opcode2 0x0) (opcode3 0x5) -> typeR
      // FF1 -> Find First 1 (opcode2 0x0) (opcode3 0xf) -> NOTYPE
      // FL1 -> Find Last 1 (opcode2 0x1) (opcode3 0xf) -> NOTYPE
      // EXTBS, // -> Extend Byte with Sign (opcode2 0x1) (opcode3 0xc) -> NOTYPE
      // EXTBZ -> Extend Byte with Zero (opcode2 0x3) (opcode3 0xc) -> NOTYPE
      // EXTHS -> Extend Half Word with Sign (opcode2 0x0) (opcode3 0xc) -> NOTYPE
      // EXTHZ -> Extend Half Word with Zero (opcode2 0x2) (opcode3 0xc) -> NOTYPE
      // EXTWS -> Extend Word with Sign (opcode2 0x0) (opcode3 0xd) -> NOTYPE
      // EXTWZ -> Extend Word with Zero (opcode2 0x1) (opcode3 0xd) -> NOTYPE
      // SLL -> Shift Left Logical (opcode2 0x0) (opcode3 0x8) -> NOTYPE
      // SRA -> Shift Right Arithmetic (opcode2 0x2) (opcode3 0x8) -> NOTYPE
      // SRL -> Shift Right Logical (opcode2 0x1) (opcode3 0x8) -> NOTYPE
  // typeF
  ADDI, // 0x27 Add Immediate
  // typeF
  ADDIC, // 0x28 Add Immediate and Carry

  ADRP, // 0x2 Compute PC-Relative Page Address -> NOTYPE

  // typeI
  ANDI, // 0x29 And with Immediate Half Word
  // typeJ
  BF, // 0x4 Branch if Flag
  // typeJ
  BNF, // 0x3 Branch if No Flag

  CSYNC, // 0x23000000 Context Synchronization -> NOTYPE (not implemented yet in the getOpcode()) 

  CUST1, // 0x1c Reserved for ORBIS32/64 Custom Instructions -> NOTYPE
  CUST2, // 0x1d Reserved for ORBIS32/64 Custom Instructions  -> NOTYPE
  CUST3, // 0x1e Reserved for ORBIS32/64 Custom Instructions -> NOTYPE
  CUST4, // 0x1f Reserved for ORBIS32/64 Custom Instructions -> NOTYPE
  CUST5, // 0x3c Reserved for ORBIS32/64 Custom Instructions -> NOTYPE
  CUST6, // 0x3d Reserved for ORBIS32/64 Custom Instructions -> NOTYPE
  CUST7, // 0x3e Reserved for ORBIS32/64 Custom Instructions -> NOTYPE
  CUST8, // 0x3f 8 Reserved for ORBIS32/64 Custom Instructions -> NOTYPE

  // typeJ
  J, // 0x0 Jump
  // typeJ
  JAL, // 0x1 Jump and Link

  JALR, // 0X12 Jump and Link Register -> NOTYPE
  JR, // 0x11 Jump Register -> NOTYPE
  // typeI
  LBS, // 0x24 Load Byte and Extend with Sign
  // typeI
  LBZ, // 0x23 Load Byte and Extend with Zero
  // typeI
  LD, // 0x20 Load Double Word 
  // typeI
  LF, // 0x1a Load Float 
  // typeI
  LHS, // 0x26 Load Half Word and Extend with Sign
  // typeI
  LHZ, // 0x25 Load Half Word and Extend with Zero
  // typeI
  LWA, // 0x1b Load Single Word Atomic
  // typeI
  LWS, // 0x22 Load Single Word and Extend with Sign
  // typeI
  LWZ, // 0x21 Load Single Word and Extend with Zero 

  // NOTYPE
  MAC, // 0x31 Multiply and Accumulate Signed (opcode2 0x1)
       // MACU -> Multiply and Accumulate Unsigned (opcode2 0x3)
       // MSB -> Multiply and Subtract Signed (opcode2 0x2)
       // MSBU -> Multiply and Subtract Unsigned (opcode2 0x4)
       
  MACI, // 0x13 -> Multiply Immediate and Accumulate Signed -> NOTYPE
  
  // NOTYPE
  MACRC, // 0x6 MAC Read and Clear (opcode2 0x10000)
        // MOVHI -> Move Immediate High (opcode2 0x0)

  // typeI
  MFSPR, // 0x2d Move From Special-Purpose Register

  // NOTYPE
  MSYNC, // 0x22000000 Memory Synchronization (not implemented yet in getOpcode()) 

  // typeS
  MTSPR, // 0x30 Move To Special-Purpose Register

  // typeI
  MULI, // 0X2c Multiply Immediate Signed

  // typeI
  ORI, // 0x2a Or with Immediate Half Word

  // NOTYPE
  PSYNC, // 0x22800000 Pipeline Synchronization (not implemented yet in getOpcode()) 

  // NOTYPE
  RFE, // 0x9 Return From Exception

  // typeSH
  RORI, // 0x2e Rotate Right with Immediate (opcode2 0x3)
        // SLLI -> Shift Left Logical with Immediate (opcode2 0x0)
        // SRAI -> Shift Right Arithmetic with Immediate (opcode2 0x2)
        // SRLI -> Shift Right Logical with Immediate (opcode2 0x1)
  
  // typeS
  SB, // 0x36 Store Byte
  SD, // 0x34 Store Double Word -> typeS

  // NOTYPE
  SFEQ, // 0x720 Set Flag if Equal (not implemented yet in getOpcode()) 

  // typeF
  SFEQI, // 0x5e0 Set Flag if Equal Immediate (not implemented yet in getOpcode()) 

  // NOTYPE
  SFGES, // 0x72b Set Flag if Greater or Equal Than Signed (not implemented yet in getOpcode()) 

  // typeF
  SFGESI, // 0x5eb Set Flag if Greater or Equal Than Immediate Signed (not implemented yet in getOpcode())

  // NOTYPE
  SFGEU, // 0x723 Set Flag if Greater or Equal Than Unsigned (not implemented yet in getOpcode())

  // typeF
  SFGEUI, // 0x5e3 Set Flag if Greater or Equal Than Immediate Unsigned (not implemented yet in getOpcode())
  
  // NOTYPE
  SFGTS, // 0x72a Set Flag if Greater Than Signed (not implemented yet in getOpcode())

  // typeF
  SFGTSI, // 0x5ea Set Flag if Greater Than Immediate Signed (not implemented yet in getOpcode())
  
  // NOTYPE
  SFGTU, // 0x722 Set Flag if Greater Than Unsigned (not implemented yet in getOpcode())

  // typeF
  SFGTUI, // 0x5e2 Set Flag if Greater Than Immediate Unsigned (not implemented yet in getOpcode())

  // NOTYPE
  SFLES, // 0x72d Set Flag if Less or Equal Than Signed (not implemented yet in getOpcode())

  // typeF
  SFLESI, // 0x5ed Set Flag if Less or Equal Than Immediate Signed (not implemented yet in getOpcode())

  // NOTYPE
  SFLEU, // 0x725 Set Flag if Less or Equal Than Unsigned (not implemented yet in getOpcode())

  // typeF
  SFLEUI, // 0x5e5 Set Flag if Less or Equal Than Immediate Unsigned (not implemented yet in getOpcode())

  // NOTYPE 
  SFLTS, //0x72c Set Flag if Less Than Signed (not implemented yet in getOpcode())

  // typeF
  SFLTSI, // 0x5ec Set Flag if Less Than Immediate Signed (not implemented yet in getOpcode())

  // NOTYPE
  SFLTU, // 0x724 Set Flag if Less Than Unsigned (not implemented yet in getOpcode())

  // typeF
  SFLTUI, //0x5e4 Set Flag if Less Than Immediate Unsigned (not implemented yet in getOpcode())

  // NOTYPE
  SFNE, // 0x721 Set Flag if Not Equal (not implemented yet in getOpcode())

  // typeF
  SFNEI, // 0x5e1 Set Flag if Not Equal Immediate (not implemented yet in getOpcode())

  // typeS
  SH, // 0x37 Store Half Word 
  // typeS
  SW , // 0x35 Store Single Word
  // typeS
  SWA, // 0x33 Store Single Word Atomic

  // NOTYPE
  SYS, // 0x2000 System call (not implemented yet in getOpcode())

  // NOTYPE
  TRAP, // 0x2100 Trap (not implemented yet in getOpcode())

  // typeI
  XORI, // 0x2b Exclusive Or with Immediate Half Word

  // NOTYPE
  NOP, // 0x15 No Operation

  END // emulator
};

enum class opcode2 
{
  // TODO find other names
  ADD, // 0x38 Add (opcode2 0x0) (opcode3 0x0)
       // ADDC -> Add and Carry (opcode2 0x0) (opcode3 0x1)
       // AND -> And (opcode2 0x0) (opcode3 0x3)
       // CMOV -> Conditional Move (opcode2 0x0) (opcode3 0xe)
       // FF1 -> Find First 1 (opcode2 0x0) (opcode3 0xf)
       // OR -> Or (opcode2 0x0) (opcode3 0x4)
       // SUB -> Subtract (opcode2 0x0) (opcode3 0x2)
       // XOR -> Exclusive Or (opcode2 0x0) (opcode3 0x5)
  FL1,  // Find Last 1 (opcode2 0x1) (opcode3 0xf)    

  DIV,   // DIV -> Divide Signed (opcode2 0x3) (opcode3 0x9)
         // DIVU -> Divide Unsigned (opcode2 0x3) (opcode3 0xa)
         // MUL -> Multiply Signed (opcode2 0x3) (opcode3 0x6)
         // MULD -> Multiply Signed to Doubl (opcode2 0x3) (opcode3 0x7)
         // MULDU -> Multiply Unsigned to Double (opcode2 0x3) (opcode3 0xc)
         // MULU -> Multiply Unsigned (opcode2 0x3) (opcode3 0xb)
         
  SLL,   // Shift Left Logical (opcode2 0x0) (opcode3 0x8)
         // EXTHS -> Extend Half Word with Sign (opcode2 0x0) (opcode3 0xc)
         // EXTWS -> Extend Word with Sign (opcode2 0x0) (opcode3 0xd)

  EXTBS, // EXTBS -> Extend Byte with Sign (opcode2 0x1) (opcode3 0xc)
         // EXTWZ -> Extend Word with Zero (opcode2 0x1) (opcode3 0xd)
         // SRL -> Shift Right Logical (opcode2 0x1) (opcode3 0x8)
       
  EXTHZ, // EXTHZ -> Extend Half Word with Zero (opcode2 0x2) (opcode3 0xc)
         // SRA -> Shift Right Arithmetic (opcode2 0x2) (opcode3 0x8)

  ROR,    // Rotate Right (opcode2 0x3) (opcode3 0x8)
          // EXTBZ -> Extend Byte with Zero (opcode2 0x3) (opcode3 0xc)

  MAC,  // 0x31 Multiply and Accumulate Signed (opcode2 0x1)
  MACU, // MACU -> Multiply and Accumulate Unsigned (opcode2 0x3)
  MSB,  // MSB -> Multiply and Subtract Signed (opcode2 0x2)
  MSBU, // MSBU -> Multiply and Subtract Unsigned (opcode2 0x4)

  RORI, // 0x2e Rotate Right with Immediate (opcode2 0x3)
  SLLI, // SLLI -> Shift Left Logical with Immediate (opcode2 0x0)
  SRAI, // SRAI -> Shift Right Arithmetic with Immediate (opcode2 0x2)
  SRLI, // SRLI -> Shift Right Logical with Immediate (opcode2 0x1)

  MACRC, // 0x6 MAC Read and Clear (opcode2 0x10000)
  MOVHI // MOVHI -> Move Immediate High (opcode2 0x0)
};


enum class opcode3
{
  // opcode2 = 0x0
  ADD,   // 0x38 Add (opcode2 0x0) (opcode3 0x0)
  ADDC,  // ADDC -> Add and Carry (opcode2 0x0) (opcode3 0x1)
  AND,   // AND -> And (opcode2 0x0) (opcode3 0x3)
  CMOV,  // CMOV -> Conditional Move (opcode2 0x0) (opcode3 0xe)
  EXTHS, // EXTHS -> Extend Half Word with Sign (opcode2 0x0) (opcode3 0xc)
  EXTWS, // EXTWS -> Extend Word with Sign (opcode2 0x0) (opcode3 0xd)
  FF1,   // FF1 -> Find First 1 (opcode2 0x0) (opcode3 0xf)
  OR,    // OR -> Or (opcode2 0x0) (opcode3 0x4)
  SUB,   // SUB -> Subtract (opcode2 0x0) (opcode3 0x2)
  XOR,   // XOR -> Exclusive Or (opcode2 0x0) (opcode3 0x5)
  SLL,   // SLL -> Shift Left Logical (opcode2 0x0) (opcode3 0x8)

  // opcode2 = 0x1
  EXTBS,  // EXTBS -> Extend Byte with Sign (opcode2 0x1) (opcode3 0xc)
  EXTWZ,  // EXTWZ -> Extend Word with Zero (opcode2 0x1) (opcode3 0xd)
  FL1,    // FL1 -> Find Last 1 (opcode2 0x1) (opcode3 0xf)
  SRL,    // SRL -> Shift Right Logical (opcode2 0x1) (opcode3 0x8)

  // opcode2 = 0x2
  EXTHZ,  // EXTHZ -> Extend Half Word with Zero (opcode2 0x2) (opcode3 0xc)
  ROR,    // ROR -> Rotate Right (opcode2 0x3) (opcode3 0x8)
  SRA,    // SRA -> Shift Right Arithmetic (opcode2 0x2) (opcode3 0x8)
  // opcode2 = 0x3
  DIV,    // DIV -> Divide Signed (opcode2 0x3) (opcode3 0x9)
  DIVU,   // DIVU -> Divide Unsigned (opcode2 0x3) (opcode3 0xa)
  EXTBZ,  // EXTBZ -> Extend Byte with Zero (opcode2 0x3) (opcode3 0xc)
  MUL,    // MUL -> Multiply Signed (opcode2 0x3) (opcode3 0x6)
  MULD,   // MULD -> Multiply Signed to Doubl (opcode2 0x3) (opcode3 0x7)
  MULDU,  // MULDU -> Multiply Unsigned to Double (opcode2 0x3) (opcode3 0xc)
  MULU,   // MULU -> Multiply Unsigned (opcode2 0x3) (opcode3 0xb)
};

enum class ALUOp
{
  ADD,
  SUB,
  AND,
  OR,
  EQ,
  GE,
  LE,
  LT,
  SRA,
  JUMP,
  // ARL,
  MOVHI,
  SLL,
  NOP
};

// enum class opcode 
// {
//   CSYNC = 0x23000000,
//   MSYNC = 0x22000000,
//   PSYNC = 0x22800000,
//   SFEQ = 0x720,
//   SFEQI = 0x5e0,
//   SFGES = 0x72b,
//   SFGESI = 0x5eb,
//   SFGEU = 0x723,
//   SFGEUI = 0x5e3,
//   SFGTS = 0x72a,
//   SFGTSI = 0x5ea,
//   SFGTU = 0x722,
//   SFGTUI = 0x5e2,
//   SFLES = 0x72d,
//   SFLESI = 0x5ed,
//   SFLEU = 0x725,
//   SFLEUI = 0x5e5,
//   SFLTS = 0x72c,
//   SFLTSI = 0x5ec,
//   SFLTU = 0x724,
//   SFLTUI = 0x5e4,
//   SFNE = 0x721,
//   SFNEI = 0x5e1,
//   SYS = 0x2000,
//   TRAP = 0x2100,
//   ADD = 0x38,
//   ADDI = 0x27,
//   ADDIC = 0x28,
//   ADRP = 0x2,
//   ANDI = 0x29,
//   BF = 0x4,
//   BNF = 0x3,
//   CUST1 = 0x1c,
//   CUST2 = 0x1d,
//   CUST3 = 0x1e,
//   CUST4 = 0x1f,
//   CUST5 = 0x3c,
//   CUST6 = 0x3d,
//   CUST7 = 0x3e,
//   CUST8 = 0x3f,
//   J = 0x0,
//   JAL = 0x1,
//   JALR = 0x12,
//   JR = 0x11,
//   LBS = 0x24,
//   LBZ = 0x23,
//   LD = 0x20,
//   LF = 0x1a,
//   LHS = 0x26,
//   LHZ = 0x25,
//   LWA = 0x1b,
//   LWS = 0x22,
//   LWZ = 0x21,
//   MAC = 0x31,
//   MACI = 0x13,
//   MACRC = 0x6,
//   MFSPR = 0x2d,
//   MTSPR = 0x30,
//   MULI = 0x2c,
//   ORI = 0x2a,
//   RFE = 0x9,
//   RORI = 0x2e,
//   SB = 0x36,
//   SD = 0x34,
//   SH = 0x37,
//   SW = 0x35,
//   SWA = 0x33,
//   XORI = 0x2b,
  
//   NOP = 0x15,
//   END,
// };

// enum class  InstructionType {typeR = 'R', typeI = 'I', typeS = 'S', typeSH = 'H', typeJ = 'J', typeF = 'F', NOTYPE = 'N'};
enum class  InstructionType {typeR, typeI, typeS, typeSH, typeJ, typeF, NOTYPE};

/* Exception that should be thrown when an illegal instruction
 * is encountered.
 */
class IllegalInstruction : public std::runtime_error
{
  public:
    explicit IllegalInstruction(const std::string &what)
      : std::runtime_error(what)
    { }

    explicit IllegalInstruction(const char *what)
      : std::runtime_error(what)
    { }
};


/* InstructionDecoder component to be used by class Processor */
class InstructionDecoder
{
  public:
    void                setInstructionWord(const uint32_t instructionWord);
    uint32_t            getInstructionWord() const;

    RegNumber           getA() const;
    RegNumber           getB() const;
    RegNumber           getD() const;

    /* TODO: probably want methods to get opcode, function code */
    opcode              getOpcode() const;  // get the opcode where it apllied
    opcode2             getOpcode2() const; // get the opcode2 where it apllied
    opcode3             getOpcode3() const; // get the opcode3 where it apllied
    InstructionType     getInstructionType() const;

    /* TODO: need a method to obtain the immediate */
    int32_t getImmediate() const;

  private:
    uint32_t instructionWord;
};

std::ostream &operator<<(std::ostream &os, const InstructionDecoder &decoder);

#endif /* __INST_DECODER_H__ */
