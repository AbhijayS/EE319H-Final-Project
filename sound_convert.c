#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#define SIZE 9999 // size of array here

const uint8_t __array__[SIZE]; // pase array here

int main(void) {
    uint8_t shit[SIZE];
    printf("const uint8_t shit[%d] = {\n", SIZE);
    for (int i = 0; i < SIZE; i++) {
        shit[i] = (__array__[i]*(1<<4))/(1<<8); // 8 bit to 4 bit conversion
        printf("%d, ", shit[i]);
        assert(shit[i] < (1<<4)); // make sure the result is actually 4 bit
    }
    printf("\n};\n");
}

