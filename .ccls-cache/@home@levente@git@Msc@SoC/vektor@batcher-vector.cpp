#include "batcher-vector.h"
#include <stdio.h>
#include "stdbool.h"
#include <cstring>

void compare(uint8_t* array, int i, int j)
{
  if (array[i]>array[j])
  {
    uint8_t tmp = array[i];
    array[i] = array[j];
    array[j] = tmp;
  }

}

void merge(uint8_t* array, int start, int length, int r)
{
  int m=r*2;
  if(m<length)
  {
    merge(array, start, length, m);
    merge(array, start+r, length, m);
    for (int i=start+r; i+r<start+length; i+=m)
      compare(array, i, i+r);
  }
  else
    compare(array, start, start+r);
}

void sort(uint8_t* array, int start, int length)
{

  if(length>1)
  {
    int m = length/2;
    sort(array, start, m);
    sort(array, start+m, m); 
    merge(array, start, length, 1);
  }

}

void media_filter_scalar(int imgHeight, int imgWidth, int imgWidthF,
			   uint8_t *imgSrcExt, uint8_t *imgDst)
{

}














void batcher-non-recursive(uint8_t* array)
{
  uint8_t inner_array[32] = {};
  for(int i=0; i<32; i++)
  {
    inner_array[i] = array[i]
  }

    

}

