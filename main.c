#include <stdio.h>
#include <stdint.h>

int getBit(uint8_t byte, int bit) {
    uint8_t shefittedByte = (byte >> (8 - bit ) );
    return (shefittedByte & 1);
}

int main() {
    uint8_t x = 1;
    printf("%d\n", getBit(x, 8));
}