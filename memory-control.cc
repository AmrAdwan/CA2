/* rv64-emu -- Simple 64-bit RISC-V simulator
 *
 *    memory-control.cc - Memory Controller
 *
 * Copyright (C) 2016-2020  Leiden University, The Netherlands.
 */

#include "memory-control.h"
#include "utils.h"
#include <iostream>

InstructionMemory::InstructionMemory(MemoryBus &bus)
  : bus(bus), size(0), addr(0)
{
}

void
InstructionMemory::setSize(const uint8_t size)
{
  if (size != 2 and size != 4)
    throw IllegalAccess("Invalid size " + std::to_string(size));
  this->size = size;
}

void
InstructionMemory::setAddress(const MemAddress addr)
{
  this->addr = addr;
}

RegValue
InstructionMemory::getValue() const
{
  switch (size)
    {
      case 2:
        return bus.readHalfWord(addr);

      case 4:
        return bus.readWord(addr);

      default:
        throw IllegalAccess("Invalid size " + std::to_string(size));
    }
}


DataMemory::DataMemory(MemoryBus &bus)
  : bus{ bus }
{
}

void
DataMemory::setSize(const uint8_t size)
{
  /* TODO: check validity of size argument */
  switch (static_cast<int8_t>(size)) 
  {
    case 1: // 1 byte
    case 2: // half word
    case 4: // word
    case 8: // double word
      this->size = size;
      break;
    default:
      throw std::invalid_argument{"Invalid size to read."};
  }
}

void
DataMemory::setAddress(const MemAddress addr)
{
  this->addr = addr;
}

void
DataMemory::setDataIn(const RegValue value)
{
  this->dataIn = value;
}

void
DataMemory::setReadEnable(bool setting)
{
  readEnable = setting;
}

void
DataMemory::setWriteEnable(bool setting)
{
  writeEnable = setting;
}

RegValue
DataMemory::getDataOut(bool signExtend) const
{
  /* TODO: implement */
  RegValue ret{0};
  switch (static_cast<int8_t>(size)) 
  {
    case 1: 
    {
      const byte_t b = bus.readByte(addr);
       ret = b;
      break;
    }
  case 2: 
  {
    const halfWord_t h = bus.readHalfWord(addr);
    ret = h;
    break;
  }
  case 4: {
    return bus.readWord(addr);
    break;
  }
  case 8: {
    ret = bus.readDoubleWord(addr);
    break;
  }
    default:
      throw std::invalid_argument{"Invalid size to read."};
  }
  return ret;
}

void
DataMemory::clockPulse() const
{
  /* TODO: implement. Write the actual value to memory when required. */
  if (!writeEnable)
    throw std::invalid_argument{"Reading is not enabled."};
  switch (static_cast<int8_t>(size)) 
  {
  case 1: 
  {
    bus.writeByte(addr, selectLowest8(dataIn));
    break;
  }
  case 2: 
  {
    bus.writeHalfWord(addr, selectLowest16(dataIn));
    break;
  }
  case 4: 
  {
    bus.writeWord(addr, selectLowest32(dataIn));
    break;
  }
  case 8: 
  {
    bus.writeDoubleWord(addr, dataIn);
    break;
  }
  default:
    throw std::invalid_argument{"Invalid size to write."};
  }
}
