// Desenvolvido por Rafael Nascimento Lourenco
// Referencias
// https://docs.fileformat.com/image/bmp/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getBit(uint8_t byte, uint8_t bit) {
  uint8_t bitsAtRigth = bit - 1;
  uint8_t shefittedByte = byte >> bitsAtRigth;
  return shefittedByte & 1;
}

#define READ_BINARY "rb"
#define BPM_HEADER_SIZE 14

#define FILE_TYPE_INDEX 0
#define FILE_TYPE_SIZE 2

#define FILE_SIZE_INDEX 2
#define BITMAP_INDEX 10

typedef struct BmpHeader {
  char fileType[2];
  uint32_t fSize;
  uint16_t reserveds[2];
  uint32_t bitmapAddress;
} bmp_header;

void getBmpHeader(char *headerBuffer, bmp_header *header) {
  memcpy(header->fileType, ((char *)headerBuffer + FILE_TYPE_INDEX),
         FILE_TYPE_SIZE);

  header->fSize = *((uint32_t *)(headerBuffer + FILE_SIZE_INDEX));

  header->bitmapAddress = *((uint32_t *)(headerBuffer + BITMAP_INDEX));
}

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    printf("Error: BMP path not found in arguments");
    exit(1);
  }

  char *filePath = (char *)argv[1];
  printf("\n%s\n", filePath);
  FILE *file = fopen(filePath, READ_BINARY);

  if (file == NULL) {
    printf("Error: Opening file");
    exit(1);
  }

  char *bhBuffer = (char *)malloc(sizeof(char) * BPM_HEADER_SIZE);
  if (bhBuffer == NULL) {
    printf("Error: Malloc return null");
    exit(1);
  }

  size_t resultSize = fread(bhBuffer, 1, BPM_HEADER_SIZE, file);
  if (resultSize != BPM_HEADER_SIZE) {
    printf("Error: Reading file");
    exit(1);
  }

  bmp_header *bh;
  getBmpHeader(bhBuffer, bh);

  fclose(file);
  free(bhBuffer);
  exit(0);
}