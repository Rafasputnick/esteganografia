#include "bmp.h"

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

bmp_info *initializeBmpInfo(int argc, char **argv) {

  bmp_info *bmpInfo;

  bmpInfo = malloc(sizeof(bmp_info));

  bmpInfo->bmpHeader = malloc(sizeof(bmp_header));
  bmpInfo->bmpHeader->fileType = malloc(sizeof(char) * 3);
  bmpInfo->bmpHeader->reserveds = malloc(sizeof(uint16_t) * 2);

  bmpInfo->dibHeader = malloc(sizeof(dib_header));

  if (argc < 4) {
    printf("Error: BMP path not found in arguments");
    exit(1);
  }

  char *filePath = (char *)argv[5];
  printf("\nReading: %s\n", filePath);
  bmpInfo->filePointer = fopen(filePath, "r+");

  if (bmpInfo->filePointer == NULL) {
    printf("%s\n", "Error: Opening file");
    fclose(bmpInfo->filePointer);
    exit(1);
  }

  getBmpHeader(bmpInfo->filePointer, bmpInfo->bmpHeader);
  getDibHeader(bmpInfo->filePointer, bmpInfo->dibHeader);

  fseek(bmpInfo->filePointer, bmpInfo->bmpHeader->bitmapAddress, SEEK_SET);

  bmpInfo->bytesInBitMap = bmpInfo->dibHeader->bmWidth *
                           bmpInfo->dibHeader->bmHeight *
                           (bmpInfo->dibHeader->bitPerPixel / 8);

  return bmpInfo;
}