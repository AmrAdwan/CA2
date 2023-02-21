/* rv64-emu -- Simple 64-bit RISC-V simulator
 *
 *    arch.h - Definitions specifying architectural properties.
 *
 * Copyright (C) 2016  Leiden University, The Netherlands.
 */

#ifndef __ARCH_H__
#define __ARCH_H__

#include <cstdint>
#include <cstddef>

/* For now these values are hard-coded for a 64-bit RISC-V
 * architecture.
 */
using RegValue = uint32_t;
using MemAddress = uint32_t;
using instruction_t = uint32_t;
using byte_t        = uint8_t;
using halfWord_t    = uint16_t;
using word_t        = uint32_t;

static constexpr size_t NumRegs = 32;
using RegNumber = uint8_t;
static constexpr size_t MaxRegs = 256;

/* Magic codeword, which is an invalid OpenRISC instruction, that we use
 * to mark the end of unit test cases. This codeword is in big-endian.
 */
static constexpr uint32_t TestEndMarker = 0x40ffccff;


#endif /* __ARCH_H__ */
