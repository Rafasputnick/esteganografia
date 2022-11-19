#ifndef BMP_H
#define BMP_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

typedef struct BmpInfo {
  FILE *filePointer;
  bmp_header *bmpHeader;
  dib_header *dibHeader;
  uint32_t bytesInBitMap;
} bmp_info;

void getBytesFromFile(FILE *filePointer, char *value, uint32_t howMuchBytes);
void getBmpHeader(FILE *filePointer, bmp_header *header);
void getDibHeader(FILE *filePointer, dib_header *header);
bmp_info *initializeBmpInfo(int argc, char **argv);

#endif