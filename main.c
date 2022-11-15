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

char **dir_list;

typedef union {
  unsigned char bytes[2];
  int16_t valor;
} byteIntoTwoBytes;

int16_t curr_dir_idx;

char **files_list;
int16_t curr_file_idx;

char **files_content;
int16_t curr_file_content_idx;

char message[256] = "Isto eh uma mensagem";

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

void initialize() {
  dir_list = malloc(sizeof(*dir_list) * 256);
  files_list = malloc(sizeof(*files_list) * 256);
  files_content = malloc(sizeof(*files_content) * 256);

  for (int i = 0; i < 256; i++) {
    dir_list[i] = malloc(sizeof(*(dir_list[i])) * 256);
    files_list[i] = malloc(sizeof(*(files_list[i])) * 256);
    files_content[i] = malloc(sizeof(*(files_content[i])) * 256);
  }
}

void freeMatrixs() {
  for (int i = 255; i >= 0; i--) {
    free(dir_list[i]);
    free(files_list[i]);
    free(files_content[i]);
  }
  free(dir_list);
  free(files_list);
  free(files_content);
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

void createLsbMethodHeader(FILE *filePointer) {
  byteIntoTwoBytes aux;
  aux.valor = -1;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      putByteWithLsbMethod(aux.bytes[j], filePointer);
    }
  }
}

void readLsbMethodHeader(FILE *filePointer) {
  byteIntoTwoBytes aux;

  aux.bytes[0] = getByteWithLsbMethod(filePointer);
  aux.bytes[1] = getByteWithLsbMethod(filePointer);
  curr_dir_idx = aux.valor;

  aux.valor = 0;

  aux.bytes[0] = getByteWithLsbMethod(filePointer);
  aux.bytes[1] = getByteWithLsbMethod(filePointer);
  curr_file_idx = aux.valor;

  aux.valor = 0;

  aux.bytes[0] = getByteWithLsbMethod(filePointer);
  aux.bytes[1] = getByteWithLsbMethod(filePointer);
  curr_file_content_idx = aux.valor;
}

int main(int argc, char *argv[]) {

  initialize();

  bmp_header *bmpHeader = malloc(sizeof(bmp_header));
  bmpHeader->fileType = malloc(sizeof(char) * 3);
  bmpHeader->reserveds = malloc(sizeof(uint16_t) * 2);

  dib_header *dibHeader = malloc(sizeof(dib_header));

  if (argc < 3) {
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

  // open and start
  char *option = (char *)argv[2];
  if ((strcmp(option, "-s") == 0)) {
    createLsbMethodHeader(filePointer);
    fseek(filePointer, bmpHeader->bitmapAddress, SEEK_SET);
  }

  readLsbMethodHeader(filePointer);

  /*
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
  */

  freeMatrixs();
  free(bmpHeader->fileType);
  free(bmpHeader->reserveds);
  free(bmpHeader);
  free(dibHeader);

  argc = 2;
  argv[1] = "\0";
  argv[0] = "\0";

  fclose(filePointer);
  exit(0);
}