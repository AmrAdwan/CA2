/* rv64-emu -- Simple 64-bit RISC-V simulator
 *
 *    inst-decoder.cc - RISC-V instruction decoder.
 *
 * Copyright (C) 2016,2019  Leiden University, The Netherlands.
 *
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"


#include "inst-decoder.h"
#include "arch.h"
#include "utils.h"

#include <iostream>
#include <map>
#include <bitset>
#include <string>


/*
 * Class InstructionDecoder -- helper class for getting specific
 * information from the decoded instruction.
 */

void
InstructionDecoder::setInstructionWord(const uint32_t instructionWord)
{
  this->instructionWord = instructionWord;
}

uint32_t
InstructionDecoder::getInstructionWord() const
{
  return instructionWord;
}

// get the opcode of each instruction
opcode InstructionDecoder::getOpcode() const
{
  if (instructionWord >> 24 == 21)
    return opcode::NOP;

  switch (instructionWord) 
  { 
    case 587202560: // 0x23000000
      return opcode::CSYNC;
    case 570425344: // 0x22000000
      return opcode::MSYNC;
    case 578813952: // 0x22800000
      return opcode::PSYNC;    
  }
  switch (instructionWord >> 21) 
  {
  case 1824: // 0x720
    return opcode::SFEQ;
  case 1504: // 0x5e0
    return opcode::SFEQI;
  case 1835: // 0x72b
    return opcode::SFGES;
  case 1515: // 0x5eb
    return opcode::SFGESI;
  case 1827: // 0x723
    return opcode::SFGEU;
  case 1507: // 0x5e3
    return opcode::SFGEUI;
  case 1834: // 0x72a
    return opcode::SFGTS;
  case 1514: // 0x5ea
    return opcode::SFGTSI;
  case 1826: // 0x722 
    return opcode::SFGTU;
  case 1506: // 0x5e2
    return opcode::SFGTUI;
  case 1837: // 0x72d
    return opcode::SFLES;
  case 1517: // 0x5ed
    return opcode::SFLESI;
  case 1829: // 0x725
    return opcode::SFLEU;
  case 1509: // 0x5e5
    return opcode::SFLEUI;
  case 1836: // 0x72c
    return opcode::SFLTS;
  case 1516: // 0x5ec
    return opcode::SFLTSI;
  case 1828: // 0x724
    return opcode::SFLTU;
  case 1508: // 0x5e4
    return opcode::SFLTUI;
  case 1825: // 0x721
    return opcode::SFNE;
  case 1505: // 0x5e1
    return opcode::SFNEI;
  }

  switch (instructionWord >> 16) 
  {
    case 8192: // 0x2000
      return opcode::SYS;
    case 8448: // 0x2100
      return opcode::TRAP;
  }


  switch (instructionWord >> 26) 
  {
    case 56: // 0x38
      return opcode::ADD;
    case 39: // 0x27 
      return opcode::ADDI;
    case 40: // 0x28
      return opcode::ADDIC;
    case 2: // 0x2
      return opcode::ADRP;
    case 41: // 0x29
      return opcode::ANDI;
    case 4: // 0x4
      return opcode::BF;
    case 3: // 0x3
      return opcode::BNF;
    case 28: // 0x1c
      return opcode::CUST1;
    case 29: // 0x1d
      return opcode::CUST2;
    case 30: // 0x1e
      return opcode::CUST3;
    case 31: // 0x1f
      return opcode::CUST4;
    case 60: // 0x3c
      return opcode::CUST5;
    case 61: // 0x3d
      return opcode::CUST6;
    case 62: // 0x3e
      return opcode::CUST7;
    case 63: // 0x3f
      return opcode::CUST8;
    case 0: // 0x0
      return opcode::J;
    case 1: // 0x1
      return opcode::JAL;
    case 18: // 0x12
      return opcode::JALR;
    case 17: // 0x11
      return opcode::JR;
    case 36: // 0x24
      return opcode::LBS;
    case 35: // 0x23
      return opcode::LBZ;
    case 32: // 0x20
      return opcode::LD;
    case 26: // 0x1a
      return opcode::LF;
    case 38: // 0x26
      return opcode::LHS;
    case 37: // 0x25
      return opcode::LHZ;
    case 27: // 0x1b
      return opcode::LWA;
    case 34: // 0x22
      return opcode::LWS;
    case 33: // 0x21
      return opcode::LWZ;
    case 49: // 0x31
      return opcode::MAC;
    case 19: // 0x13
      return opcode::MACI;
    case 6: // 0x6
      return opcode::MACRC;
    case 45: // 0x2d
      return opcode::MFSPR;
    case 48: // 0x30
      return opcode::MTSPR;
    case 44: // 0x2c
      return opcode::MULI;
    case 42: // 0x2a
      return opcode::ORI;
    case 9: // 0x9
      return opcode::RFE;
    case 46: // 0x2e
      return opcode::RORI;
    case 54: // 0x36 
      return opcode::SB;
    case 52: // 0x34
      return opcode::SD;
    case 55: // 0x37
      return opcode::SH;
    case 53: // 0x35
      return opcode::SW;
    case 51: // 0x33
      return opcode::SWA;
    case 43: // 0x2b
      return opcode::XORI;
    // case 21: // 0x15
    //   return opcode::NOP;
    default:
      if (instructionWord == TestEndMarker) //|| instructionWord == NOPMarker ||
          // instructionWord == STALLMarker)// || instructionWord >> 24 == 21)
          return opcode::END;
      throw IllegalInstruction{"Illegal or unsupported opcode."};
  }
}


// to get the opcode 2 of some of the instructions that could not be identified 
// using the getopcode function
opcode2
InstructionDecoder::getOpcode2() const 
{
  const opcode opcode = getOpcode();
  if (opcode == opcode::RORI)
  {
    // startBit = 6;
    // lastBit = 7; 
    // maskSign2 = ((1 << (7 - 6 + 1)) - 1) << 6;
    // switch (bitMask(6, 7)) 
    switch (selectBits32_8(instructionWord, 6, 7, 0))
    {
    
      case 0: // 0x0
        return opcode2::SLLI;
      case 1: // 0x1
        return opcode2::SRLI;
      case 2: // 0x2
        return opcode2::SRAI;
      case 3: // 0x3
        return opcode2::RORI;
    }
  }  

  else if (opcode == opcode::ADD) 
  {
    if (selectBits32_8(instructionWord, 6, 9, 0) == 2
        && selectBits32_8(instructionWord, 0, 3, 0) == 8)
      return opcode2::EXTHZ;


    switch (selectBits32_8(instructionWord, 8, 9, 0))
    {
      case 0: // 0x0
        if (selectBits32_8(instructionWord, 6, 9, 0) == 0
            && selectBits32_8(instructionWord, 0, 3, 0) == 8)
          return opcode2::SLL;
        return opcode2::ADD;
      case 1:
        if (selectBits32_8(instructionWord, 6, 9, 0) == 1)
          return opcode2::EXTBS;
        return opcode2::FL1;
      case 3:
        if (selectBits32_8(instructionWord, 6, 9, 0) == 3)
          return opcode2::ROR;
        return opcode2::DIV;
    }
  }
  else if (opcode == opcode::ADD) 
  {
    switch (selectBits32_8(instructionWord, 6, 9, 0))
    {
      case 0: // 0x0
        return opcode2::SLL;
      case 1: // 0x1
        return opcode2::EXTBS;
      case 2: // 0x2
        return opcode2::EXTHZ;
      case 3: // 0x3
        return opcode2::ROR;
    }
  }
  else if (opcode == opcode::MACRC)
  {
    if (selectBits32_8(instructionWord, 16, 16, 0) == 0)
      return opcode2::MOVHI;
    return opcode2::MACRC;
  }
//   else {
    //   throw IllegalInstruction{"Illegal or unsupported opcode."};
//   }
  return static_cast<opcode2>(0);
}

// to get the opcode 3 of some of the instructions that could not be identified 
// using the getopcode2 function
opcode3
InstructionDecoder::getOpcode3() const
{
  opcode2 opcode2 = getOpcode2();
  if (opcode2 == opcode2::ADD)
  {
    switch (selectBits32_8(instructionWord, 0, 3, 0))
    {
      case 0: // 0x0
        return opcode3::ADD;
      case 1: // 0x1
        return opcode3::ADDC;
      case 2: // 0x2
        return opcode3::SUB;
      case 3: // 0x3
        return opcode3::AND;
      case 4: // 0x4
        return opcode3::OR;
      case 5: // 0x5
        return opcode3::XOR;
      case 14: // 0xe
        return opcode3::CMOV;
      case 15: // 0xf
        return opcode3::FF1;
      case 8: // 0x8
        return opcode3::ROR;
    }
  }
  else if (opcode2 == opcode2::DIV) 
  {
    switch (selectBits32_8(instructionWord, 0, 3, 0))
    {
      case 6: // 0x6
        return opcode3::MUL;
      case 7: // 0x7
        return opcode3::MULD;
      case 9: // 0x9
        return opcode3::DIV;
      case 10: // 0xa
        return opcode3::DIVU;
      case 11: // 0xb
        return opcode3::MULU;
      case 12: // 0xc
        return opcode3::MULDU;
    }
  }
  else if (opcode2 == opcode2::SLL) 
  {
    switch (selectBits32_8(instructionWord, 0, 3, 0))
    {
      case 8: // 0x8
        return opcode3::SLL;
      case 12: // 0xc
        return opcode3::EXTHS;
      case 13: // 0xd
        return opcode3::EXTWS;
    }
  }
  else if (opcode2 == opcode2::EXTBS) 
  {
    switch (selectBits32_8(instructionWord, 0, 3, 0))
    {
      case 8: // 0x8
        return opcode3::SRL;
      case 12: // 0xc
        return opcode3::EXTBS;
      case 13: // 0xd
        return opcode3::EXTWZ;
    }
  }
  else if (opcode2 == opcode2::EXTHZ) 
  {
    switch (selectBits32_8(instructionWord, 0, 3, 0))
    {
      case 8:
        return opcode3::SRA;
      case 12:
        return opcode3::EXTHZ;
    }
  }
  else if (opcode2 == opcode2::ROR) 
  {
    switch (selectBits32_8(instructionWord, 0, 3, 0)) 
    {
      case 8:
        return opcode3::ROR;
      case 12:
        return opcode3::EXTBZ;
    }
  }
  return static_cast<opcode3>(0);
//   throw IllegalInstruction{"Illegal or unsupported opcode."};
}


// get the instruction type of each instruction where applicable
InstructionType
InstructionDecoder::getInstructionType() const
{
  opcode opcode = getOpcode();
  opcode2 opcode2 = getOpcode2();
  opcode3 opcode3 = getOpcode3();

  switch (opcode) 
  {
    // typeF instructions
    case opcode::ADDI:
    case opcode::ADDIC:
    case opcode::SFEQI:
    case opcode::SFGESI:
    case opcode::SFGEUI:
    case opcode::SFGTSI:
    case opcode::SFGTUI:
    case opcode::SFLESI:
    case opcode::SFLEUI:
    case opcode::SFLTSI:
    case opcode::SFLTUI:
    case opcode::SFNEI:
      return InstructionType::typeF;
    
    // typeT instructions
    case opcode::ANDI:
    case opcode::LBS:
    case opcode::LBZ:
    case opcode::LD:
    case opcode::LF:
    case opcode::LHS:
    case opcode::LHZ:
    case opcode::LWA:
    case opcode::LWS:
    case opcode::LWZ:
    case opcode::MFSPR:
    case opcode::MULI:
    case opcode::ORI:
    case opcode::XORI:
      return InstructionType::typeI;
    
    // typeS instructions
    case opcode::MTSPR:
    case opcode::SB:
    case opcode::SD:
    case opcode::SH:
    case opcode::SW:
    case opcode::SWA:
      return InstructionType::typeS;

    // typeJ
    case opcode::BF:
    case opcode::BNF:
    case opcode::J:
    case opcode::JAL:
      return InstructionType::typeJ;

    // case opcode::ADRP:
    case opcode::MACI:
    case opcode::NOP:
    case opcode::SYS:
    case opcode::TRAP:
      return InstructionType::NOTYPE;
  }

  switch (opcode2) 
  {
   //typeSH instructions
   case opcode2::RORI:
   case opcode2::SLLI:
   case opcode2::SRAI:
   case opcode2::SRLI:
    return InstructionType::typeSH;
  case opcode2::MOVHI:
    return InstructionType::NOTYPE; 
  }

  switch (opcode3) 
  {
    case opcode3::ADD:
    case opcode3::ADDC:
    case opcode3::AND:
    case opcode3::CMOV:
    case opcode3::DIV:
    case opcode3::DIVU:
    case opcode3::MUL:
    case opcode3::MULU:
    case opcode3::OR:
    case opcode3::SUB:
    case opcode3::XOR:
      return InstructionType::typeR;
  }
//   throw IllegalInstruction{"Illegal or unsupported opcode."};
  return static_cast<InstructionType>(0);
}

RegNumber
InstructionDecoder::getA() const
{
  /* TODO: implement */
  // gets the bits from position 16 to 20 to get the value of A
  // startBit = 16;
  // lastBit = 20; 
  return selectBits32_8(instructionWord, 16, 20, 0);


  // return 0;  /* result undefined */
}

RegNumber
InstructionDecoder::getB() const
{
  /* TODO: implement */
  // gets the bits from position 11 to 15 to get the value of B
  // startBit = 11;
  // lastBit = 15; 
  // std::cout << "getBBBBBBBBBBBBB = = = " << selectBits32_8(instructionWord, 11, 15, 0) << '\n';
  return selectBits32_8(instructionWord, 11, 15, 0);

  // return 0;  /* result undefined */
}

RegNumber
InstructionDecoder::getD() const
{
  /* TODO: implement */
  // gets the bits from position 21 to 25 to get the value of D
  // startBit = 21;
  // lastBit = 25; 
  return selectBits32_8(instructionWord, 21, 25, 0);

  // return 0; /* result undefined */
}


// get the immediate of each type
int32_t
InstructionDecoder::getImmediate() const
{ 
  int32_t imm = 0, imm_2 = 0;
  int32_t imm_1 = 0;
  std::string res;
  if (getOpcode() == opcode::ADRP)
  {
    imm = selectBits32_8(instructionWord, 0, 20, 0);
    if (imm >> 20 == 0x1)
    {
      imm |= 0b11111111111110000000000000000000;
    }
  }

  else if (getOpcode() == opcode::MACRC)
  {
    if (getOpcode2() == opcode2::MOVHI)
    {
      imm = selectBits32_8(instructionWord, 0, 15, 0);
      // imm = imm << 16;
    }
  }

  else 
  {
    switch (getInstructionType())
    {
      // add this for instructions that do not have type and have immediate
      case InstructionType::NOTYPE:
      case InstructionType::typeF:
      case InstructionType::typeI:
        imm = selectBits32_8(instructionWord, 0, 15, 0);
        if (imm >> 15 == 0x1) 
        {
          imm |= 0b11111111111111110000000000000000;
        }
        break;  
      case InstructionType::typeSH:
        imm = selectBits32_8(instructionWord, 0, 5, 0);
        if (imm >> 5 == 0x1)
        {
          imm |= 0b11111111111111111111111111000000;
        }
        break;
      case InstructionType::typeJ:
        imm = selectBits32_8(instructionWord, 0, 25, 0);
        if (imm >> 25 == 0x1)
        {
          imm |= 0b11111100000000000000000000000000;
        }
        break;
      case InstructionType::typeS:
        imm_1 = (instructionWord >> 21) & 0b11111;
        imm_2 = instructionWord & 0b11111111111;
        imm = (imm_1 << 11) | imm_2;
        if (imm >> 15 == 0x1)
        {
          imm |= 0b11111111111111110000000000000000;
        }
        break;
    }
  }
  
  return imm;
}

#pragma GCC diagnostic pop
