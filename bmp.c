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

void exitWithError(FILE *filePointer, char *msg) {
  printf("%s\n", msg);
  fclose(filePointer);
  exit(1);
}
