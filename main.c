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
  FILE *filePointer = fopen(filePath, "r+");


  if (filePointer == NULL) {
    exitWithError(filePointer, "Error: Opening file");
  }

  getBmpHeader(filePointer, bmpHeader);
  getDibHeader(filePointer, dibHeader);

  fseek(filePointer, bmpHeader->bitmapAddress, SEEK_SET);

  uint32_t bytesInBitMap =
      dibHeader->bmWidth * dibHeader->bmHeight * (dibHeader->bitPerPixel / 8);

  for (int i = 0; i < 10000; i++) {

    uint8_t teste;
    fread(&teste, 1, 1, filePointer);

    fseek(filePointer, -1, SEEK_CUR);

    if (getBit(teste, 1) == 0) {
      teste += 1;
    }

    fputc(teste, filePointer);

    fseek(filePointer, -1, SEEK_CUR);

    fread(&teste, 1, 1, filePointer);

    printf("%d\n", teste);
  }

  free(bmpHeader->fileType);
  free(bmpHeader->reserveds);
  free(bmpHeader);
  free(dibHeader);

  fclose(filePointer);
  exit(0);
}