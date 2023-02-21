#include "utils.h"
#include <iostream>

uint32_t bitMask(unsigned int first, unsigned int last) 
{
  if (first > last)
    throw std::out_of_range{"First bit must not be after last bit."};
  if (last >= 8 * sizeof(uint32_t))
    throw std::out_of_range{"Last bit out of range."};
  if (last - first >= 8 * sizeof(uint32_t))
    throw std::out_of_range{"Shift count out of range."};
  return ((1 << (last - first + 1)) - 1) << first;
}

int32_t selectBits32_8(const uint32_t input, unsigned int first,
                       unsigned int last, unsigned int shift) 
{
  // first > last already tested in bitMask32().
  if (last >= 32)
    throw std::out_of_range{"Last bit out of range."};
  // if ( shift + last - first >= 8 )
  //     throw std::out_of_range{"Shift count out of range."};
  const uint32_t selected = input & bitMask(first, last);
  return (selected >> first) << shift;
}

// function to convert decimal to hexadecimal
std::string decToHexa(int n) 
{
  // ans string to store hexadecimal number
  std::string ans = "";

  while (n != 0) 
  {
    // remainder variable to store remainder
    int rem = 0;

    // ch variable to store each character
    char ch;
    // storing remainder in rem variable.
    rem = n % 16;

    // check if temp < 10
    if (rem < 10) 
    {
      ch = rem + 48;
    } else 
    {
      ch = rem + 55;
    }

    // updating the ans string with the character variable
    ans += ch;
    n = n / 16;
  }

  // reversing the ans string to get the final result
  int i = 0, j = ans.size() - 1;
  while (i <= j) 
  {
    std::swap(ans[i], ans[j]);
    i++;
    j--;
  }
  return ans;
}


// bool getBit (uint32_t n, uint8_t bit)
// {
//   if ( bit >= 8 * sizeof(uint32_t) )
//     throw std::out_of_range{"Bit out of range."};
//   std::cout << " (n >> bit) & 0x1 = " << (n & 0x1) << " !!??";
//   return (n >> bit) & 0x1;
//   // return n & 0x1;
// }

// uint32_t signExtend(uint32_t val, uint8_t start)
// {
//    return uint64_t{val} | ((~(uint64_t{0})) << start); 
// }

int32_t signExtend8(int32_t val)
{
  return val |= 0b11111111111111111111111100000000;
}
int32_t signExtend16(int32_t val)
{
  return val |= 0b11111111111111110000000000000000;
}
// int32_t signExtend32(int32_t val)
// {
//   return val |= 0b11111111111110000000000000000000;
// }

// int32_t signExtend26(int32_t val)
// {
//   return val |= 0b11111100000000000000000000000000;;
// }



uint8_t selectLowest8( uint64_t val ) {
    return val & mask8;
}

uint16_t selectLowest16( uint64_t val ) {
    return val & mask16;
}

uint32_t selectLowest32( uint64_t val ) {
    return val & mask32;
}