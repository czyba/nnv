#include "basic_math.h"

inline long long int min(long long int x, long long int y) {
  if (x < y) {
    return x;
  }
  return y;
}

inline long long int max(long long int x, long long int y) {
  if (x > y) {
    return x;
  }
  return y;
}

inline unsigned long long int minu(unsigned long long int x, unsigned long long int y) {
  if (x < y) {
    return x;
  }
  return y;
}

inline unsigned long long int maxu(unsigned long long int x, unsigned long long int y) {
  if (x > y) {
    return x;
  }
  return y;
}

unsigned long long int next_pow_2(unsigned long long int x) {
  int max_shift = sizeof(x) << 2;
  x--;
  for (int i = 1; i <= max_shift; i <<= 1) {
    x |= (x >> i);
  }
  x++;
  return x;
}
