#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int getBit(uint8_t byte, int bit) {
  uint8_t bitsAtRigth =  bit - 1;
  uint8_t shefittedByte = byte >> bitsAtRigth;
  return shefittedByte & 1;
}

int main(int argc, const char *argv[]) {
  printf("%d\n", argc);
  for (int i = 0; i < argc; i++) {
    printf("%s\n", argv[i]);
  }

  exit(0);
}