// Linked List strucutre used from COMP20023 Project 1 (ADS 2022 Sem 2) by Milad Izra

#ifndef _UTIL_H_
#define _UTIL_H_

#include <assert.h>
#include "rpc.h"
#include <stdlib.h>


struct array {
    rpc_function **F;
    int size;
    int n;
};

typedef struct array array_t;

array_t *createArray();

void ensureArraySize(array_t *arr);

// void appendArray(array_t *arr, footpath_t *n);

void arrayFree(array_t *arr);

#endif