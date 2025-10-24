#include "batcher.h"
#include <stdio.h>
#include "stdbool.h"

void compare(uint8_t* array, int i, int j)
{
  if (array[i]>array[j])
  {
    int tmp = array[i];
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

      sort(r_vals,0,25);
      sort(g_vals,0,25);
      sort(b_vals,0,25);

			int wr_offset;
			wr_offset = row*imgWidth*3 + col*3;

      
      *(imgDst + wr_offset + 0) = r_vals[12];
      *(imgDst + wr_offset + 1) = g_vals[12];
      *(imgDst + wr_offset + 2) = b_vals[12];

    }
  }



}
