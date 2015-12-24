#include "basic_math.h"

inline long long int min(long long int x, long long int y) {
  if(x < y) {
    return x;
  }
  return y;
}


inline long long int max(long long int x, long long int y){
  if (x > y) {
    return x;
  }
  return y;
}


inline unsigned long long int minu(unsigned long long int x, unsigned long long int y) {
  if(x < y) {
    return x;
  }
  return y;
}


inline unsigned long long int maxu(unsigned long long int x, unsigned long long int y){
  if (x > y) {
    return x;
  }
  return y;
}
