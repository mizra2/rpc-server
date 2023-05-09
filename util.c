// Linked List strucutre used from COMP20003 Project 1 (ADS 2022 Sem 2) by Milad Izra

#include <stdio.h>
#include "util.h"



array_t *createArray() {
    array_t *arr = malloc(sizeof(*arr));
    assert(arr);
    int size = 2;
    arr->size = size;
    arr->F = malloc(size * sizeof(*(arr->F)));
    assert(arr->F);
    arr->n = 0;
    return arr;
}


void ensureArraySize(array_t *arr) {
    if(arr->n == arr->size) {
        arr->size *= 2;
        arr->F = realloc(arr->F, arr->size * sizeof(*(arr->F)));
        assert(arr->F);
    }
}


// void appendArray(array_t *arr, /*TODO ADD FUNCTION TO ARRAY OF FUNCTIONS*/ ) {
//     ensureArraySize(arr);
//     arr->F[arr->n] = n;
//     (arr->n)++;
// }

void arrayFree(array_t *arr) {
	for (int i = 0; i < arr->n; i++) {
		free(arr->F[i]);
	}
	free(arr->F);
	free(arr);
}