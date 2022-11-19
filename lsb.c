#include "lsb.h"

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

void readLsbMethodHeader(FILE *filePointer) {
  setLshHeaderValue(&curr_dir_idx);
  setLshHeaderValue(&curr_file_idx);
  setLshHeaderValue(&curr_file_content_idx);
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

void readContentInFIle(FILE *filePointer) {
  fseek(filePointer, getIndexValue(DIRR_IDX, 0), SEEK_SET);
  setLshContentValue(dir_list, curr_dir_idx);

  fseek(filePointer, getIndexValue(FILE_IDX, 0), SEEK_SET);
  setLshContentValue(files_list, curr_file_idx);

  fseek(filePointer, getIndexValue(FILE_CONTENT_IDX, 0), SEEK_SET);
  setLshContentValue(files_content_list, curr_file_content_idx);
}

void updateLsbHeader(uint8_t index) {
  switch (index) {
  case DIRR_IDX:
    fseek(filePointer,
          (bmpHeader->bitmapAddress + (DIRR_IDX * sizeof(int16_t) * 8)),
          SEEK_SET);
    updateLsbHeaderValue(curr_dir_idx);
    break;
  case FILE_IDX:
    fseek(filePointer,
          (bmpHeader->bitmapAddress + (FILE_IDX * sizeof(int16_t) * 8)),
          SEEK_SET);
    updateLsbHeaderValue(curr_file_idx);
    break;
  case FILE_CONTENT_IDX:
    fseek(filePointer,
          (bmpHeader->bitmapAddress + (FILE_CONTENT_IDX * sizeof(int16_t) * 8)),
          SEEK_SET);
    updateLsbHeaderValue(curr_file_content_idx);
    break;
  }
}

void updateLsbContent(char *content) {
  for (int i = 0; i < 8; i++) {
    putByteWithLsbMethod(content[i], filePointer);
  }
}