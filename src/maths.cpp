#include <maths.h>

size_t highest_one_bit(size_t n) {
  // 4 byte << 2 -> 16
  for (size_t i = 1; i < (sizeof(n) << 2); i <<= 1) {
    n |= (n >> i);
  }
  return n - (n >> 1);
}

size_t next_power_of_two(size_t n) {
  return highest_one_bit(n) == n ? n : n << 1;
}