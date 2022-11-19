#include "lsb.h"
#include <stdint.h>

uint32_t getIndexValue(int matrixIndex, uint16_t curIndex, uint32_t bitmapAddress) {
  uint32_t list = MATRIX_SIZE * matrixIndex;
  uint32_t index = MATRIX_LENGHT * curIndex;
  return (uint32_t)(bitmapAddress + ((LSB_HEADER_SIZE + list + index) * BYTE));
}

int getBit(uint8_t byte, uint8_t bit) {
  uint8_t bitsAtRigth = bit - 1;
  uint8_t shefittedByte = byte >> bitsAtRigth;
  return shefittedByte & 1;
}

void putByteWithLsbMethod(int8_t byteForLsb, FILE *filePointer) {
  int8_t cur_byte;
  for (int i = 1; i < 9; i++) {
    fread(&cur_byte, 1, 1, filePointer);
    fseek(filePointer, -1, SEEK_CUR);
    if (getBit(byteForLsb, i)) {
      if (cur_byte % 2 == 0) {
        cur_byte++;
      }
    } else {
      if (cur_byte % 2 == 1) {
        cur_byte--;
      }
    }
    fputc(cur_byte, filePointer);
  }
}

int8_t getByteWithLsbMethod(FILE *filePointer) {
  int8_t res = 0;
  int8_t byteFromFile;

  for (int i = 0; i < 8; i++) {
    fread(&byteFromFile, 1, 1, filePointer);
    if (byteFromFile % 2 == 1) {
      res |= 1 << i;
    }
  }

  return res;
}

void updateLsbHeaderValue(int16_t valor, FILE *filePointer) {
  byteIntoTwoBytes aux;
  aux.valor = valor;

  for (int j = 0; j < 2; j++) {
    putByteWithLsbMethod(aux.bytes[j], filePointer);
  }
}

void createLsbMethodHeader(FILE *filePointer) {
  updateLsbHeaderValue(-1, filePointer);
  updateLsbHeaderValue(-1, filePointer);
  updateLsbHeaderValue(-1, filePointer);
}

void setLshHeaderValue(int16_t *value, FILE *filePointer) {
  byteIntoTwoBytes aux;

  aux.bytes[0] = getByteWithLsbMethod(filePointer);
  aux.bytes[1] = getByteWithLsbMethod(filePointer);
  *value = aux.valor;
}

void setLshContentValue(char **value, int16_t maxIndex, FILE *filePointer) {
  char byte;
  for (int i = 0; i <= maxIndex; i++) {
    for (int j = 0; j < 8; j++) {
      byte = getByteWithLsbMethod(filePointer);
      value[i][j] = byte;
    }
  }
}

void readContentInFIle(FILE *filePointer, char **matrix, int matrixIndex, int16_t arrayIndex, uint32_t bitmapAddress) {
  fseek(filePointer, getIndexValue(matrixIndex, 0, bitmapAddress), SEEK_SET);
  setLshContentValue(matrix, arrayIndex, filePointer);
}

void updateLsbHeader(uint8_t index, FILE *filePointer, int matrixIndex, int16_t arrayIndex, uint32_t bitmapAddress ) {
  fseek(filePointer,
        (bitmapAddress + (matrixIndex * sizeof(int16_t) * BYTE)),
        SEEK_SET);
  updateLsbHeaderValue(arrayIndex, filePointer);

}

void updateLsbContent(char *content, FILE *filePointer) {
  for (int i = 0; i < 8; i++) {
    putByteWithLsbMethod(content[i], filePointer);
  }
}