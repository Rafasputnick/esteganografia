
#ifndef LSB_H
#define LSB_H

#include <stdio.h>
#include <stdint.h>

#define MATRIX_LENGHT 10
#define MATRIX_SIZE MATRIX_LENGHT *MATRIX_LENGHT
#define LSB_HEADER_SIZE 6
#define BYTE 8

typedef union {
  unsigned char bytes[2];
  int16_t valor;
} byteIntoTwoBytes;

uint32_t getIndexValue(int matrixIndex, uint16_t curIndex, uint32_t bitmapAddress);

#endif