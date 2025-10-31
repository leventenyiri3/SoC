#include "batcher.h"
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
  for(int row=0; row<imgHeight; row++)
  {
    for(int col=0; col<imgWidthF; col++)
    {
      uint8_t r_vals[25], g_vals[25], b_vals[25];
			int rd_offset;

      for(int fy=0; fy<5; fy++)
      {
        for(int fx=0; fx<5; fx++)
        {
          rd_offset = ((row+fy)*imgWidthF + (col+fx))*3;
          int idx = fy*5+fx;

          r_vals[idx] = *(imgSrcExt + rd_offset + 0);
          g_vals[idx] = *(imgSrcExt + rd_offset + 1);
          b_vals[idx] = *(imgSrcExt + rd_offset + 2);
        }
      }
      uint8_t padded_r[32], padded_g[32], padded_b[32];
      memcpy(padded_r, r_vals, 25);
      memset(padded_r + 25, 255, 7);
      memcpy(padded_g, g_vals, 25);
      memset(padded_g + 25, 255, 7);
      memcpy(padded_b, b_vals, 25);
      memset(padded_b + 25, 255, 7);

      sort(padded_r,0,32);
      sort(padded_g,0,32);
      sort(padded_b,0,32);

			int wr_offset;
			wr_offset = row*imgWidth*3 + col*3;
      
      *(imgDst + wr_offset + 0) = padded_r[12];
      *(imgDst + wr_offset + 1) = padded_g[12];
      *(imgDst + wr_offset + 2) = padded_b[12];

    }
  }
}
