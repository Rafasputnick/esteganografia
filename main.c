// Desenvolvido por Rafael Nascimento Lourenco
// Referencias
// https://docs.fileformat.com/image/bmp/
// https://stackoverflow.com/questions/14279242/read-bitmap-file-into-structure
// https://en.wikipedia.org/wiki/BMP_file_format

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
#define DIB_HEADER_SIZE 12

#define FILE_TYPE_INDEX 0
#define FILE_TYPE_SIZE 2

#define FILE_SIZE_INDEX 2
#define BITMAP_INDEX 10

#define HEADER_INDEX 0
#define BM_WIDTH_INDEX 4
#define BM_HEIGHT_INDEX 6
#define COLOR_PLANES_INDEX 8
#define BIT_PER_PIXEL_INDEX 10

typedef struct BmpHeader {
  char fileType[2];
  uint32_t fSize;
  uint16_t reserveds[2];
  uint32_t bitmapAddress;
} bmp_header;

typedef struct DibHeader {
  uint32_t headerSize;
  uint16_t bmWidth;
  uint16_t bmHeight;
  uint16_t colorPlanes;
  uint16_t bitPerPixel;
} dib_header;

void exitWithError(FILE *file, char *msg) {
  printf("%s\n", msg);
  fclose(file);
  exit(1);
}

void getBmpHeader(char *headerBuffer, bmp_header *header) {
  memcpy(header->fileType, ((char *)(headerBuffer + FILE_TYPE_INDEX)),
         FILE_TYPE_SIZE);

  header->fSize = *((uint32_t *)(headerBuffer + FILE_SIZE_INDEX));

  header->bitmapAddress = *((uint32_t *)(headerBuffer + BITMAP_INDEX));
}

void getDibHeader(char *headerBuffer, dib_header *header) {

  header->headerSize = *((uint32_t *)(headerBuffer + HEADER_INDEX));

  header->bmWidth = *((uint16_t *)(headerBuffer + BM_WIDTH_INDEX));

  header->bmHeight = *((uint16_t *)(headerBuffer + BM_HEIGHT_INDEX));

  header->colorPlanes = *((uint16_t *)(headerBuffer + COLOR_PLANES_INDEX));

  header->bitPerPixel = *((uint16_t *)(headerBuffer + BIT_PER_PIXEL_INDEX));
}

int main(int argc, const char *argv[]) {
  size_t resultSize;
  bmp_header *bh = malloc(sizeof(bmp_header));
  dib_header *dh = malloc(sizeof(dib_header));

  if (argc < 2) {
    printf("Error: BMP path not found in arguments");
    exit(1);
  }

  char *filePath = (char *)argv[1];
  printf("\n%s\n", filePath);
  FILE *file = fopen(filePath, READ_BINARY);

  if (file == NULL) {
    exitWithError(file, "Error: Opening file");
  }

  char *bhBuffer = (char *)malloc(sizeof(char) * BPM_HEADER_SIZE);
  if (bhBuffer == NULL) {
    exitWithError(file, "Error: Malloc return null");
  }

  resultSize = fread(bhBuffer, 1, BPM_HEADER_SIZE, file);
  if (resultSize != BPM_HEADER_SIZE) {
    exitWithError(file, "Error: Reading file");
  }

  getBmpHeader(bhBuffer, bh);

  char *dhBuffer = (char *)malloc(sizeof(char) * DIB_HEADER_SIZE);
  if (dhBuffer == NULL) {
    exitWithError(file, "Error: Malloc return null");
  }

  resultSize = fread(dhBuffer, 1, DIB_HEADER_SIZE, file);
  if (resultSize != DIB_HEADER_SIZE) {
    exitWithError(file, "Error: Reading file");
  }

  getDibHeader(bhBuffer, dh);

  fclose(file);
  free(bh);
  free(dh);
  free(bhBuffer);
  free(dhBuffer);
  exit(0);
}