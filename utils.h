#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>


static constexpr uint8_t       mask8 = ~uint8_t{0};
static constexpr uint16_t     mask16 = ~uint16_t{0};
static constexpr uint32_t     mask32 = ~uint32_t{0};



uint32_t bitMask( unsigned int first, unsigned int last );

int32_t selectBits32_8( const uint32_t input, unsigned int first,
                        unsigned int last, unsigned int shift );

std::string decToHexa(int n); 

// Select the lowest 8 bits of the given value.
uint8_t selectLowest8( uint64_t val );

// Select the lowest 16 bits of the given value.
uint16_t selectLowest16( uint64_t val );

// Select the lowest 32 bits of the given value.
uint32_t selectLowest32( uint64_t val );

// bool getBit (uint32_t n, uint8_t bit);

// uint32_t signExtend(uint32_t val, uint8_t start);
int32_t signExtend8(int32_t val);
int32_t signExtend16(int32_t val);
// int32_t signExtend32(int32_t val);


#endif  // __UTILS_H__
