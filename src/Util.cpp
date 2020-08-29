#include "Util.h"

namespace Util {
unsigned int nextPow2(unsigned int x) {
  --x;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  return x + 1;
}

void endianSwap(int32_t *value) {
  unsigned char *chs;
  unsigned char temp;

  chs = (unsigned char *) value;

  temp = chs[0];
  chs[0] = chs[3];
  chs[3] = temp;
  temp = chs[1];
  chs[1] = chs[2];
  chs[2] = temp;
}

void endianSwap(int16_t *value) {
  unsigned char *chs;
  unsigned char temp;

  chs = (unsigned char *) value;

  temp = chs[0];
  chs[0] = chs[1];
  chs[1] = temp;
}

void endianSwap(uint16_t *value) {
  endianSwap((int16_t *) value);
}
}// namespace Util
