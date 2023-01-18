#include "unpack.h"

uint8_t bit(uint8_t *data, int bit)
{
  return (*data >> bit) & 1;
}
