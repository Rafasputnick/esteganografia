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
  char *fileType;
  uint32_t fSize;
  uint16_t *reserveds;
  uint32_t bitmapAddress;
} bmp_header;

typedef struct DibHeader {
  uint32_t headerSize;
  uint16_t bmWidth;
  uint16_t bmHeight;
  uint16_t colorPlanes;
  uint16_t bitPerPixel;
} dib_header;

void exitWithError(FILE *filePointer, char *msg) {
  printf("%s\n", msg);
  fclose(filePointer);
  exit(1);
}

void getBytesFromFile(FILE *filePointer, char *value, uint32_t howMuchBytes) {
  fread(value, 1, howMuchBytes, filePointer);
}

void getBmpHeader(FILE *filePointer, bmp_header *header) {
  getBytesFromFile(filePointer, header->fileType, 2);

  getBytesFromFile(filePointer, ((char *)&header->fSize), 4);

  getBytesFromFile(filePointer, ((char *)header->reserveds), 2);
  getBytesFromFile(filePointer, ((char *)header->reserveds + 1), 2);

  getBytesFromFile(filePointer, ((char *)&header->bitmapAddress), 4);
}

void getDibHeader(FILE *filePointer, dib_header *header) {

  getBytesFromFile(filePointer, ((char *)&header->headerSize), 4);

  getBytesFromFile(filePointer, ((char *)&header->bmWidth), 4);

  getBytesFromFile(filePointer, ((char *)&header->bmHeight), 4);

  getBytesFromFile(filePointer, ((char *)&header->colorPlanes), 2);

  getBytesFromFile(filePointer, ((char *)&header->bitPerPixel), 2);
}

int main(int argc, const char *argv[]) {
  bmp_header *bmpHeader = malloc(sizeof(bmp_header));
  bmpHeader->fileType = malloc(sizeof(char) * 3);
  bmpHeader->reserveds = malloc(sizeof(uint16_t) * 2);

  dib_header *dibHeader = malloc(sizeof(dib_header));

  if (argc < 2) {
    printf("Error: BMP path not found in arguments");
    exit(1);
  }

  char *filePath = (char *)argv[1];
  printf("\nReading: %s\n", filePath);
  FILE *filePointer = fopen(filePath, READ_BINARY);

  if (filePointer == NULL) {
    exitWithError(filePointer, "Error: Opening file");
  }

  getBmpHeader(filePointer, bmpHeader);
  getDibHeader(filePointer, dibHeader);

  free(bmpHeader->fileType);
  free(bmpHeader->reserveds);
  free(bmpHeader);
  free(dibHeader);

  fclose(filePointer);
  exit(0);
}