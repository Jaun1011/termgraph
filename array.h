#include <stdio.h>

#define sizeof_array(a) sizeof(a) / sizeof(a[0])

typedef struct Array {
    size_t size;
    size_t dimensions;
    void *value;
} Array;
