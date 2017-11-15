#pragma once 

// Struct for dynamic array
typedef struct
{
    int* array;
    int size;
    int capacity;
} VectorInt;

// Inits vector
void initVectorInt(VectorInt* vec);

// Adds to vector
void addToVectorInt(VectorInt* vec, int val);
