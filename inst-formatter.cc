/* rv64-emu -- Simple 64-bit RISC-V simulator
 *
 *    inst-formatter.cc - RISC-V instruction printer (disassembler)
 *
 * Copyright (C) 2016,2018  Leiden University, The Netherlands.
 */


#include "arch.h"
#include "inst-decoder.h"

#include <functional>
#include <map>
#include <iostream>
#include <ostream>
#include <string>

std::string printRegisterNumber(RegNumber reg ) 
// RegValue printRegisterNumber( RegNumber reg ) 
{
    
  // TODO: If we want we could print the alternate names of the registers.
  return "r" + std::to_string(reg);
  // return reg;
}

std::string printRA( const InstructionDecoder & decoder ) 
{
    return printRegisterNumber(decoder.getA());
}

std::string printRB( const InstructionDecoder & decoder ) 
{
    return printRegisterNumber(decoder.getB());
}


std::string printRD( const InstructionDecoder & decoder ) 
{
    return printRegisterNumber(decoder.getD());
}

std::string printImmediate (const InstructionDecoder & decoder)
{
  // std::cout << "bbbbbbbbbbmnbmnbmnbmnb" << std::endl;
  return std::to_string(decoder.getImmediate());
}

void printADD(std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.add " << printRD(decoder) << ", " << printRA(decoder) << " " << printRB(decoder); 
}

void printSUB(std::ostream & os, const InstructionDecoder & decoder)
{
 os << "l.sub " << printRD(decoder) << ", " << printRA(decoder) << ", " << printRB(decoder);  
}

void printADDC(std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.addc " << printRD(decoder) << ", " << printRA(decoder) << ", " << printRB(decoder);
}

void printAND(std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.and " << printRD(decoder) << ", " << printRA(decoder) << " " << printRB(decoder); 
}

void printCMOV(std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.cmov " << printRD(decoder) << ", " << printRA(decoder) << " " << printRB(decoder); 
}

void printEXTHS (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.exths " << printRD(decoder) << ", " << printRA(decoder) << " " << printRB(decoder); 
}

void printEXTWS(std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.extws " << printRD(decoder) << ", " << printRA(decoder);
}

void printFF1(std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.ff1 " << printRD(decoder) << ", " << printRA(decoder);
}

void printOR(std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.or " << printRD(decoder) << ", " << printRA(decoder) << " " << printRB(decoder); 
}

void printXOR(std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.xor " << printRD(decoder) << ", " << printRA(decoder) << " " << printRB(decoder); 
}

void printSLL(std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sll " << printRD(decoder) << ", " << printRA(decoder) << ", " << printRB(decoder); 
}

void printFL1 (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.fl1 " << printRD(decoder) << ", " << printRA(decoder);
}

void printDIV (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.div " << printRD(decoder) << ", " << printRA(decoder) << " " << printRB(decoder);
}

void printDIVU (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.divu " << printRD(decoder) << ", " << printRA(decoder) << " " << printRB(decoder);
}

void printEXTBZ (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.extbz " << printRD(decoder) << ", " << printRA(decoder);
}

void printMUL (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.mul " << printRD(decoder) << ", " << printRA(decoder) << " " << printRB(decoder);
}

void printMULD(std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.muld " << printRA(decoder) << ", " << printRB(decoder);
}

void printMULDU(std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.muldu " << printRA(decoder) << ", " << printRB(decoder);
}

void printMULU (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.mulu " << printRD(decoder) << ", " << printRA(decoder) << " " << printRB(decoder);
}

void printROR (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.ror " << printRD(decoder) << ", " << printRA(decoder) << " " << printRB(decoder);
}

void printSW (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sw " << printRB(decoder) << ", " << printImmediate(decoder) << "(" <<  printRA(decoder) << ")";
}

void printNOP(std::ostream & os, const InstructionDecoder & decoder) 
{
  os << "l.nop " << "$" << printImmediate(decoder);
}

void printEXTBS(std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.extbs " << printRD(decoder) << ", " << printRA(decoder) << ", " << "$" << printImmediate(decoder);
}

void printEXTWZ(std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.extwz " << printRD(decoder) << ", " << printRA(decoder);
}

void printSRL(std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.srl " << printRD(decoder) << ", " << printRA(decoder) << ", " << printRB(decoder); 
}

void printEXTHZ (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.exthz " << printRD(decoder) << ", " << printRA(decoder);
}

void printSRA (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sra " << printRD(decoder) << ", " << printRA(decoder) << ", " << printRB(decoder);
}

void printADDI(std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.addi " << printRD(decoder) << ", " << printRA(decoder) << ", " << "$" << printImmediate(decoder);
}

void printADDIC (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.addic " << printRD(decoder) << ", " << printRA(decoder) << ", " << "$" << printImmediate(decoder);
}

void printADRP (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.adrp " << printRD(decoder) << ", " << "$" << printImmediate(decoder);
}

void printANDI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.andi " << printRD(decoder) << ", " << printRA(decoder) << ", " << "$" << printImmediate(decoder);
}

void printBF (std::ostream & os, const InstructionDecoder & decoder)
{
 os << "l.bf " << "$" << printImmediate(decoder);   
}

void printBNF (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.bnf " << "$" << printImmediate(decoder);
}

void printCSYNC (std::ostream & os)
{
  os << "l.csync ";
}

void printCUST1 (std::ostream & os)
{
  os << "l.cust1 ";
}

void printCUST2 (std::ostream & os)
{
  os << "l.cust2 ";
}

void printCUST3 (std::ostream & os)
{
  os << "l.cust3 ";
}

void printCUST4 (std::ostream & os)
{
  os << "l.cust4 ";
}

void printCUST5 (std::ostream & os)
{
  os << "l.cust4 ";
}

void printCUST6 (std::ostream & os)
{
  os << "l.cust5 ";
}

void printCUST7 (std::ostream & os)
{
  os << "l.cust6 ";
}

void printCUST8 (std::ostream & os)
{
  os << "l.cust7 ";
}

void printJ (std::ostream & os, const InstructionDecoder & decoder)
{
 os << "l.j " << "$" << printImmediate(decoder);   
}

void printJAL (std::ostream & os, const InstructionDecoder & decoder)
{
 os << "l.jal " << "$" << printImmediate(decoder);   
}

void printJALR (std::ostream & os, const InstructionDecoder & decoder)
{
 os << "l.jalr " << printRB(decoder); 
}

void printJR (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.jr " << printRB(decoder);
}

void printLBS (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.lbs " << printRD(decoder) << ", " << printImmediate(decoder) << "(" <<  printRA(decoder) << ")";
}

void printLBZ (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.lbz " << printRD(decoder) << ", " << printImmediate(decoder) << "(" <<  printRA(decoder) << ")";
}

void printLD (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.ld " << printRD(decoder) << ", " << printImmediate(decoder) << "(" <<  printRA(decoder) << ")";
}

void printLF (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.lf " << printRD(decoder) << ", " << printImmediate(decoder) << "(" <<  printRA(decoder) << ")";
}

void printLHS (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.lhs " << printRD(decoder) << ", " << printImmediate(decoder) << "(" <<  printRA(decoder) << ")";
}

void printLHZ (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.lhz " << printRD(decoder) << ", " << printImmediate(decoder) << "(" <<  printRA(decoder) << ")";
}

void printLWA (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.lwa " << printRD(decoder) << ", " << printImmediate(decoder) << "(" <<  printRA(decoder) << ")";
}

void printLWS (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.lws " << printRD(decoder) << ", " << printImmediate(decoder) << "(" <<  printRA(decoder) << ")";
}

void printLWZ (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.lwz " << printRD(decoder) << ", " << printImmediate(decoder) << "(" <<  printRA(decoder) << ")";
}

void printMAC (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.mac " << printRA(decoder) << ", " << printRB(decoder);
}


void printMACU (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.macu " << printRA(decoder) << ", " << "$" << printImmediate(decoder);
}


void printMSB (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.msb " << printRA(decoder) << ", " << printRB(decoder);
}

void printMSBU (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.msbu " << printRA(decoder) << ", " << printRB(decoder);
}

void printMACI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.maci " << printRA(decoder) << ", " << "$" << printImmediate(decoder);
}


void printMACRC (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.macrc " << printRD(decoder);
}
          
void printMOVHI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.movhi " << printRD(decoder) << ", " << "$" << printImmediate(decoder);
}

void printMFSPR (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.mfspr " << printRD(decoder) << ", " << printRA(decoder) << ", " << "$" << printImmediate(decoder);
}

void printMSYNC (std::ostream & os)
{
  os << "l.msync ";
}

void printMTSPR (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.mtspr " << printRA(decoder) << ", " << printRB(decoder) << ", " << "$" << printImmediate(decoder);
}

void printMULI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.muli " << printRD(decoder) << ", " << printRA(decoder) << ", " << "$" << printImmediate(decoder);
}

void printORI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.ori " << printRD(decoder) << ", " << printRA(decoder) << ", " << "$" << printImmediate(decoder);
}

void printPSYNC (std::ostream & os)
{
  os << "l.psync ";
}

void printRFE (std::ostream & os)
{
  os << "l.rfe ";
}

void printRORI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.rori " << printRD(decoder) << ", " << printRA(decoder) << " $" << printImmediate(decoder);
}

void printSLLI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.slli " << printRD(decoder) << ", " << printRA(decoder) << " $" << printImmediate(decoder);
}
void printSRAI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.srai " << printRD(decoder) << ", " << printRA(decoder) << " $" << printImmediate(decoder);
}

void printSRLI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.srli " << printRD(decoder) << ", " << printRA(decoder) << " $" << printImmediate(decoder);
}

void printSB(std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sb " << printRB(decoder) << ", " << printImmediate(decoder) << "(" <<  printRA(decoder) << ")";
}

void printSD (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sd " << printRB(decoder) << ", " << printImmediate(decoder) << "(" <<  printRA(decoder) << ")";
}

void printSFEQ (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sfeq  " << printRA(decoder) << ", " << printRB(decoder);
}

void printSFEQI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sfeqi " << printRA(decoder) << " $" << printImmediate(decoder);
}

void printSFGES (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sfges " << printRA(decoder) << ", " << printRB(decoder);
}

void printSFGESI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sfgesi " << printRA(decoder) << " $" << printImmediate(decoder);
}

void printSFGEU (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sfgeu " << printRA(decoder) << ", " << printRB(decoder);
}

void printSFGEUI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sfgeui " << printRA(decoder) << " $" << printImmediate(decoder);
}

void printSFGTS (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sfgts " << printRA(decoder) << ", " << printRB(decoder);
}

void printSFGTSI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sfgtsi " << printRA(decoder) << " $" << printImmediate(decoder);
}

void printSFGTU (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sfgtu " << printRA(decoder) << ", " << printRB(decoder);
}

void printSFGTUI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sfgtui " << printRA(decoder) << " $" << printImmediate(decoder);
}

void printSFLES (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sfles " << printRA(decoder) << ", " << printRB(decoder);
}

void printSFLESI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sflesi " << printRA(decoder) << " $" << printImmediate(decoder);
}

void printSFLEU (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sfleu " << printRA(decoder) << ", " << printRB(decoder);
}

void printSFLEUI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sfleui " << printRA(decoder) << " $" << printImmediate(decoder);
}

void printSFLTS (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sflts " << printRA(decoder) << ", " << printRB(decoder);
}

void printSFLTSI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sfltsi " << printRA(decoder) << " $" << printImmediate(decoder);
}


void printSFLTU (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sfltu " << printRA(decoder) << ", " << printRB(decoder);
}


void printSFLTUI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sfltui " << printRA(decoder) << " $" << printImmediate(decoder);
}

void printSFNE (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sfne " << printRA(decoder) << ", " << printRB(decoder);
}

void printSFNEI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sfneI " << printRA(decoder) << " $" << printImmediate(decoder);
}

void printSH (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sh " << printRB(decoder) << ", " << printImmediate(decoder) << "(" <<  printRA(decoder) << ")";
}

void printSWA (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.swa " << printRB(decoder) << ", " << printImmediate(decoder) << "(" <<  printRA(decoder) << ")";
}

void printSYS (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.sys " << " $" << printImmediate(decoder);
}

void printTRAP (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.trap " << " $" << printImmediate(decoder);
}

void printXORI (std::ostream & os, const InstructionDecoder & decoder)
{
  os << "l.xori " << printRD(decoder) << ", " << printRA(decoder) << " $" << printImmediate(decoder);
}





std::ostream &
operator<<(std::ostream &os, const InstructionDecoder &decoder)
{
  // char sign = ' ';
  /* TODO: write a textual representation of the decoded instruction
   * in "decoder" to the output stream "os". Do not include a newline.
   * And remove the statement below.
   */
  // os << "not implemented";

  switch ( decoder.getOpcode() ) 
  {
    case opcode::ADD:
      switch (decoder.getOpcode2()) 
      {
        case opcode2::ADD:
          switch (decoder.getOpcode3()) 
          {
            case opcode3::ADD:
              printADD(os, decoder);
              break;
            case opcode3::SUB:
              printSUB(os, decoder);
              break;
            case opcode3::ADDC:
              printADDC(os, decoder);
              break;
            case opcode3::AND:
              printAND(os, decoder);
              break;
            case opcode3::CMOV:
              printCMOV(os, decoder);
              break;
            case opcode3::FF1:
              printFF1(os, decoder);
              break;
            case opcode3::OR:
              printOR(os, decoder);
              break;
            case opcode3::XOR:
              printXOR(os, decoder);
              break;
          }
          break;
          case opcode2::DIV:
            switch (decoder.getOpcode3()) 
            {
              case opcode3::DIV:
                printDIV(os, decoder);
                break;
              case opcode3::DIVU:
                printDIVU(os, decoder);
                break;
              case opcode3::EXTBZ:
                printEXTBZ(os, decoder);
                break;
              case opcode3::MUL:
                printMUL(os, decoder);
                break;
              case opcode3::MULD:
                printMULD(os, decoder);
                break;
              case opcode3::MULDU:
                printMULDU(os, decoder);
                break;
              case opcode3::MULU:
                printMULU(os, decoder);
                break;
            }
          case opcode2::SLL:
            switch (decoder.getOpcode3()) 
            {
              case opcode3::SLL:
                printSLL(os, decoder);
                break;
              case opcode3::EXTHS:
                printEXTHS(os, decoder);
                break;
              case opcode3::EXTWS:
                printEXTWS(os, decoder);
                break;
            }
          break;
          case opcode2::EXTBS:
            switch (decoder.getOpcode3()) 
            {
              case opcode3::EXTBS:
                printEXTBS(os, decoder);
                break;
              case opcode3::EXTWZ:
                printEXTWZ(os, decoder);
                break;
              case opcode3::SRL:
                printSRL(os, decoder);
                break;
              case opcode3::FL1:
                printFL1(os, decoder);
                break; 
            }
          case opcode2::EXTHZ:
            switch (decoder.getOpcode3()) 
            {
              case opcode3::EXTHZ:
                printEXTHZ(os, decoder);
                break;
              case opcode3::SRA:
                printSRA(os, decoder);
                break;
              case opcode3::ROR:
                printROR(os, decoder);
                break;
            }
      }
    break;
    case opcode::ADDI:
      printADDI(os, decoder);
      break;
    case opcode::ADDIC:
      printADDIC(os, decoder);
      break;
    case opcode::ADRP:
      printADRP(os, decoder);
      break;
    case opcode::ANDI:
      printANDI(os, decoder);
      break;
    case opcode::BF:
      printBF(os, decoder);
      break;
    case opcode::BNF:
      printBNF(os, decoder);
      break;
    case opcode::CSYNC:
      printCSYNC(os);
      break;
    case opcode::CUST1:
      printCUST1(os);
      break;
    case opcode::CUST2:
      printCUST2(os);
      break;
    case opcode::CUST3:
      printCUST3(os);
      break;
    case opcode::CUST4:
      printCUST4(os);
      break;
    case opcode::CUST5:
      printCUST5(os);
      break;
    case opcode::CUST6:
      printCUST6(os);
      break;
    case opcode::CUST7:
      printCUST7(os);
      break;
    case opcode::CUST8:
      printCUST8(os);
      break;
    case opcode::J:
      printJ(os, decoder);
      break;
    case opcode::JAL:
      printJAL(os, decoder);
      break;
    case opcode::JALR:
      printJALR(os, decoder);
      break;
    case opcode::JR:
      printJR(os, decoder);
      break;
    case opcode::LBS:
      printLBS(os, decoder);
      break;
    case opcode::LBZ:
      printLBZ(os, decoder);
      break;
    case opcode::LD:
      printLD(os, decoder);
      break;      
    case opcode::LF:
      printLF(os, decoder);
      break; 
    case opcode::LHS:
      printLHS(os, decoder);
      break; 
    case opcode::LHZ:
      printLHZ(os, decoder);
      break;
    case opcode::LWA:
      printLWA(os, decoder);
      break;
    case opcode::LWS:
      printLWS(os, decoder);
      break;
    case opcode::LWZ:
      printLWZ(os, decoder);
      break;
    case opcode::MAC:
      switch (decoder.getOpcode2()) 
      {
        case opcode2::MAC:
          printMAC(os, decoder);
          break;
        case opcode2::MACU:
          printMACU(os, decoder);
          break;
        case opcode2::MSB:
          printMSB(os, decoder);
          break;
        case opcode2::MSBU:
          printMSBU(os, decoder);
          break;
      }
      break;
    case opcode::MACI:
      printMACI(os, decoder);
      break;
    case opcode::MACRC:
      switch (decoder.getOpcode2()) 
      {
        case opcode2::MACRC:
          printMACRC(os, decoder);
          break;
        case opcode2::MOVHI:
          printMOVHI(os, decoder);
          break;
      }
      break;
    case opcode::MFSPR:
      printMFSPR(os, decoder);
      break;
    case opcode::MSYNC:
      printMSYNC(os);
      break;
    case opcode::MTSPR:
      printMTSPR(os, decoder);
      break;
    case opcode::MULI:
      printMULI(os, decoder);
      break;
    case opcode::ORI:
      printORI(os, decoder);
      break;
    case opcode::PSYNC:
      printPSYNC(os);
      break;
    case opcode::RFE:
      printRFE(os);
      break;
    case opcode::RORI:
      switch (decoder.getOpcode2()) 
      {
        case opcode2::RORI:
          printRORI(os, decoder);
          break;
        case opcode2::SLLI:
          printSLLI(os, decoder);
          break;
        case opcode2::SRAI:
          printSRAI(os, decoder);
          break;
        case opcode2::SRLI:
          printSRLI(os, decoder);
          break;
      }
    break;
    case opcode::SB:
      printSB(os, decoder);
      break;
    case opcode::SD:
      printSD(os, decoder);
      break;
    case opcode::SFEQ:
      printSFEQ(os, decoder);
      break;
    case opcode::SFEQI:
      printSFEQI(os, decoder);
      break;
    case opcode::SFGES:
      printSFGES(os, decoder);
      break;
    case opcode::SFGESI:
      printSFGESI(os, decoder);
      break;
    case opcode::SFGEU:
      printSFGEU(os, decoder);
      break;
    case opcode::SFGEUI:
      printSFGEUI(os, decoder);
      break;
    case opcode::SFGTS:
      printSFGTS(os, decoder);
      break;
    case opcode::SFGTSI:
      printSFGTSI(os, decoder);
      break;
    case opcode::SFGTU:
      printSFGTU(os, decoder);
      break;
    case opcode::SFGTUI:
      printSFGTUI(os, decoder);
      break;
    case opcode::SFLES:
      printSFLES(os, decoder);
      break;
    case opcode::SFLESI:
      printSFLESI(os, decoder);
      break;
    case opcode::SFLEU:
      printSFLEU(os, decoder);
      break;
    case opcode::SFLEUI:
      printSFLEUI(os, decoder);
      break;
    case opcode::SFLTS:
      printSFLTS(os, decoder);
      break;
    case opcode::SFLTSI:
      printSFLTSI(os, decoder);
      break;
    case opcode::SFLTU:
      printSFLTU(os, decoder);
      break;
    case opcode::SFLTUI:
      printSFLTUI(os, decoder);
      break;
    case opcode::SFNE:
      printSFNE(os, decoder);
      break;
    case opcode::SFNEI:
      printSFNEI(os, decoder);
      break;
    case opcode::SH:
      printSH(os, decoder);
      break;
     break;
    case opcode::SW:
      printSW(os, decoder);
      break;
    case opcode::SWA:
      printSWA(os, decoder);
      break;
    case opcode::SYS:
      printSYS(os, decoder);
      break;
    case opcode::TRAP:
      printTRAP(os, decoder);
      break;
    case opcode::XORI:
      printXORI(os, decoder);
      break;
    case opcode::NOP:
      printNOP(os, decoder);
      break;
  }
  return os;
}


void printEMU( std::ostream & os, const InstructionDecoder & decoder )
{
    if ( decoder.getInstructionWord() == TestEndMarker )
        os << "TEST-END";
    else
        throw IllegalInstruction{"Invalid or unsupported opcode."};
}