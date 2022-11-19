
#ifndef LSB_H
#define LSB_H

#include <stdint.h>
#include <stdio.h>

#define MATRIX_LENGHT 10
#define MATRIX_SIZE MATRIX_LENGHT *MATRIX_LENGHT
#define LSB_HEADER_SIZE 6
#define BYTE 8

typedef union {
  unsigned char bytes[2];
  int16_t valor;
} byteIntoTwoBytes;

uint32_t getIndexOfBmpWithLsb(int matrixIndex, uint16_t arrayIndex, uint32_t bitmapAddress);

int getBit(uint8_t byte, uint8_t bit);

void putByteInFileWithLsb(int8_t byteForLsb, FILE *filePointer);

int8_t getByteInFileWithLsb(FILE *filePointer);

void putIndexInFileWithLsb(int16_t indexValue, FILE *filePointer);

void createIndexesHeaderInFile(FILE *filePointer);

void setIndexHeaderPointer(int16_t *value, FILE *filePointer);

void readMatrixContentInFIle(FILE *filePointer, char **matrix, int lastMatrixIndex, int16_t arrayIndex, uint32_t bitmapAddress);

void setIndexHeaderInFile(FILE *filePointer, int matrixIndex, int16_t indexValue, uint32_t bitmapAddress);

void setMatrixContentInFile(char *content, FILE *filePointer);

#endif