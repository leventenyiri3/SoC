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

void batcher_non_recursive(uint8_t* array)
{
  uint8_t merge_8, merge_16, merge_16_for, merge_32, base = 0;
  merge_8 = 0;
  merge_16 = 0;
  merge_16_for = 0;
  merge_32 = 0;
  base = 0;

  #pragma unroll
  for(int base=0; base<32; base += 4)
  {
    if(base != 0 && base%8 == 0)
    {
      compare(array, merge_8, merge_8+4);
      compare(array, merge_8+2, merge_8+6);
      compare(array, merge_8+2, merge_8+4);
      compare(array, merge_8+1, merge_8+5);
      compare(array, merge_8+3, merge_8+7);
      compare(array, merge_8+3, merge_8+5);
      compare(array, merge_8+1, merge_8+2);
      compare(array, merge_8+3, merge_8+4);
      compare(array, merge_8+5, merge_8+6);

      merge_8 += 8;
    }

    if(merge_8 != 0 && base%16 == 0)
    {
      compare(array, merge_16, merge_16+8);
      compare(array, merge_16+4, merge_16+12);
      compare(array, merge_16+4, merge_16+8);
      compare(array, merge_16+2, merge_16+10);
      compare(array, merge_16+6, merge_16+14);
      compare(array, merge_16+6, merge_16+10);
      compare(array, merge_16+2, merge_16+4);
      compare(array, merge_16+6, merge_16+8);
      compare(array, merge_16+10, merge_16+12);

      merge_16 += 1;

      compare(array, merge_16, merge_16+8);
      compare(array, merge_16+4, merge_16+12);
      compare(array, merge_16+4, merge_16+8);
      compare(array, merge_16+2, merge_16+10);
      compare(array, merge_16+6, merge_16+14);
      compare(array, merge_16+6, merge_16+10);
      compare(array, merge_16+2, merge_16+4);
      compare(array, merge_16+6, merge_16+8);
      compare(array, merge_16+10, merge_16+12);

      merge_16 = 16;

      compare(array, merge_16_for+1, merge_16_for+2);
      compare(array, merge_16_for+3, merge_16_for+4);
      compare(array, merge_16_for+5, merge_16_for+6);
      compare(array, merge_16_for+7, merge_16_for+8);
      compare(array, merge_16_for+9, merge_16_for+10);
      compare(array, merge_16_for+11, merge_16_for+12);
      compare(array, merge_16_for+13, merge_16_for+14); 

      merge_16_for += 16;
    }

    compare(array, base, base+1);
    compare(array, base+2, base+3);
    compare(array, base, base+2);
    compare(array, base+1, base+3);
    compare(array, base+1, base+2);

    

  }

  compare(array, merge_8, merge_8+4);
  compare(array, merge_8+2, merge_8+6);
  compare(array, merge_8+2, merge_8+4);
  compare(array, merge_8+1, merge_8+5);
  compare(array, merge_8+3, merge_8+7);
  compare(array, merge_8+3, merge_8+5);
  compare(array, merge_8+1, merge_8+2);
  compare(array, merge_8+3, merge_8+4);
  compare(array, merge_8+5, merge_8+6);


  compare(array, merge_16, merge_16+8);
  compare(array, merge_16+4, merge_16+12);
  compare(array, merge_16+4, merge_16+8);
  compare(array, merge_16+2, merge_16+10);
  compare(array, merge_16+6, merge_16+14);
  compare(array, merge_16+6, merge_16+10);
  compare(array, merge_16+2, merge_16+4);
  compare(array, merge_16+6, merge_16+8);
  compare(array, merge_16+10, merge_16+12);

  merge_16 += 1;

  compare(array, merge_16, merge_16+8);
  compare(array, merge_16+4, merge_16+12);
  compare(array, merge_16+4, merge_16+8);
  compare(array, merge_16+2, merge_16+10);
  compare(array, merge_16+6, merge_16+14);
  compare(array, merge_16+6, merge_16+10);
  compare(array, merge_16+2, merge_16+4);
  compare(array, merge_16+6, merge_16+8);
  compare(array, merge_16+10, merge_16+12);

  merge_16 = 16;

  compare(array, merge_16_for+1, merge_16_for+2);
  compare(array, merge_16_for+3, merge_16_for+4);
  compare(array, merge_16_for+5, merge_16_for+6);
  compare(array, merge_16_for+7, merge_16_for+8);
  compare(array, merge_16_for+9, merge_16_for+10);
  compare(array, merge_16_for+11, merge_16_for+12);
  compare(array, merge_16_for+13, merge_16_for+14); 

  #pragma unroll
  for(int merge_32=0; merge_32<2; merge_32++)
    {
      compare(array, merge_32+0, merge_32+16);
      compare(array, merge_32+8, merge_32+24);
      compare(array, merge_32+8, merge_32+16);
      compare(array, merge_32+8, merge_32+24);
      compare(array, merge_32+4, merge_32+20);
      compare(array, merge_32+12, merge_32+28);
      compare(array, merge_32+12, merge_32+20);
      compare(array, merge_32+4, merge_32+8);
      compare(array, merge_32+12, merge_32+16);
      compare(array, merge_32+20, merge_32+24);
      compare(array, merge_32+2, merge_32+18);
      compare(array, merge_32+10, merge_32+26);
      compare(array, merge_32+8, merge_32+24);
      compare(array, merge_32+10, merge_32+18);
      compare(array, merge_32+6, merge_32+22);
      compare(array, merge_32+14, merge_32+30);
      compare(array, merge_32+14, merge_32+22);
      compare(array, merge_32+6, merge_32+10);
      compare(array, merge_32+14, merge_32+18);
      compare(array, merge_32+22, merge_32+26);

      compare(array, merge_32+2, merge_32+4);
      compare(array, merge_32+6, merge_32+8);
      compare(array, merge_32+10, merge_32+12);
      compare(array, merge_32+14, merge_32+16);
      compare(array, merge_32+18, merge_32+20);
      compare(array, merge_32+22, merge_32+24);
      compare(array, merge_32+26, merge_32+28);
    }

  #pragma unroll
  for(int i=1; i<30; i+=2)
  {
    compare(array, i, i+1);
  }


}

void media_filter_vector(int imgHeight, int imgWidth, int imgWidthF,
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

      batcher_non_recursive(padded_r);
      batcher_non_recursive(padded_g);
      batcher_non_recursive(padded_b);

			int wr_offset;
			wr_offset = row*imgWidth*3 + col*3;
      
      *(imgDst + wr_offset + 0) = padded_r[12];
      *(imgDst + wr_offset + 1) = padded_g[12];
      *(imgDst + wr_offset + 2) = padded_b[12];

    }
  }
}