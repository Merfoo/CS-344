#include <stdlib.h>
#include "VectorInt.h"

void initVectorInt(VectorInt* vec)
{
    vec->capacity = 4;
    vec->size = 0;
    vec->array = malloc(vec->capacity * sizeof(int));
}

void addToVectorInt(VectorInt* vec, int val)
{
    // Double the array capacity if the size is >= to capacity
    if(vec->size >= vec->capacity)
    {
        // Double capacity and create new array with capacity length
        vec->capacity *= 2;
        int* newArray = malloc(vec->capacity * sizeof(int));

        // Copy over the old array values into new bigger one
        for(int i = 0; i < vec->size; i++)
            newArray[i] = vec->array[i];

        // Free old array and set old one to point to new bigger one
        free(vec->array);
        vec->array = newArray;
    }

    vec->array[vec->size] = val;
    vec->size++;
}

