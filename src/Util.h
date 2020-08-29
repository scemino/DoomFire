#ifndef COLORCYCLING__UTIL_H
#define COLORCYCLING__UTIL_H

#include <cstdint>

namespace Util {
void endianSwap(int32_t *value);
void endianSwap(int16_t *value);
void endianSwap(uint16_t *value);
unsigned int nextPow2(unsigned int x);
}// namespace Util

#endif//COLORCYCLING__UTIL_H
