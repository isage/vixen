#ifndef __BIT_H__
#define __BIT_H__

inline uint8_t bit(uint8_t *data, int bit)
{
  return (*data >> bit) & 1;
}

#endif