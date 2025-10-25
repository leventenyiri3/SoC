#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void media_filter_scalar(int imgHeight, int imgWidth, int imgWidthF,
			   uint8_t *imgSrcExt, uint8_t *imgDst);

void sort(uint8_t* array, int start, int length);

void merge(uint8_t* array, int start, int length, int r);

void compare(uint8_t* array, int i, int j);


void batcher_non_recursive(uint8_t* array);

void media_filter_vector(int imgHeight, int imgWidth, int imgWidthF,
			   uint8_t *imgSrcExt, uint8_t *imgDst);