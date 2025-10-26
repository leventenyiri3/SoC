#include "batcher.h"
#include <stdio.h>
#include "stdbool.h"
#include <cstring>
#include <immintrin.h>


__m256i gather_bytes_manual(const uint8_t* memory, const uint32_t* indices) {
    uint8_t temp[32];
    for (int i = 0; i < 32; i++) {
        temp[i] = memory[indices[i]];
    }
    return _mm256_loadu_si256((__m256i*)temp);
}

void batcher()
{
  


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
