#include "batcher.h"
#include <stdio.h>
#include "stdbool.h"
#include <cstring>
#include <emmintrin.h>
#include <nmmintrin.h>
#include <immintrin.h>


inline void compare_exchange(__m256i &a, __m256i &b) {
    __m256i lo = _mm256_min_epu8(a, b);
    __m256i hi = _mm256_max_epu8(a, b);
    a = lo;
    b = hi;
}

void media_filter_vector(int imgHeight, int imgWidth, int imgWidthF,
			   uint8_t *imgSrcExt, uint8_t *imgDst)
{
  __m256i load_data[25] = {};

  for (int32_t row=0; row<imgHeight; row++)
  {
    for(int32_t col=0; col<imgWidthF*3; col = col + 32)
    {

      int32_t rd_offset = ((row)*imgWidthF*3 + col);
      
      // Loading all 25 vectors (Cannot be skipped as all are needed for the median)
      load_data[0] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 0 + rd_offset));
      load_data[1] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 3 + rd_offset));
      load_data[2] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 6 + rd_offset));
      load_data[3] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 9 + rd_offset));
      load_data[4] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 12 + rd_offset));
      load_data[5] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 0 + rd_offset + imgWidthF*3));
      load_data[6] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 3 + rd_offset + imgWidthF*3));
      load_data[7] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 6 + rd_offset + imgWidthF*3));
      load_data[8] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 9 + rd_offset + imgWidthF*3));
      load_data[9] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 12 + rd_offset + imgWidthF*3));
      load_data[10] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 0 + rd_offset + imgWidthF*6));
      load_data[11] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 3 + rd_offset + imgWidthF*6));
      load_data[12] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 6 + rd_offset + imgWidthF*6));
      load_data[13] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 9 + rd_offset + imgWidthF*6));
      load_data[14] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 12 + rd_offset + imgWidthF*6));
      load_data[15] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 0 + rd_offset + imgWidthF*9));
      load_data[16] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 3 + rd_offset + imgWidthF*9));
      load_data[17] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 6 + rd_offset + imgWidthF*9));
      load_data[18] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 9 + rd_offset + imgWidthF*9));
      load_data[19] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 12 + rd_offset + imgWidthF*9));
      load_data[20] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 0 + rd_offset + imgWidthF*12));
      load_data[21] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 3 + rd_offset + imgWidthF*12));
      load_data[22] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 6 + rd_offset + imgWidthF*12));
      load_data[23] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 9 + rd_offset + imgWidthF*12));
      load_data[24] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + 12 + rd_offset + imgWidthF*12));

      // Optimized Comparison Sequence for Median (Index 12)
      compare_exchange(load_data[0], load_data[1]);
      compare_exchange(load_data[2], load_data[3]);
      compare_exchange(load_data[0], load_data[2]);
      compare_exchange(load_data[1], load_data[3]);
      compare_exchange(load_data[1], load_data[2]);
      compare_exchange(load_data[4], load_data[5]);
      compare_exchange(load_data[6], load_data[7]);
      compare_exchange(load_data[4], load_data[6]);
      compare_exchange(load_data[5], load_data[7]);
      compare_exchange(load_data[5], load_data[6]);
      compare_exchange(load_data[0], load_data[4]);
      compare_exchange(load_data[2], load_data[6]);
      compare_exchange(load_data[2], load_data[4]);
      compare_exchange(load_data[1], load_data[5]);
      compare_exchange(load_data[3], load_data[7]);
      compare_exchange(load_data[3], load_data[5]);
      compare_exchange(load_data[1], load_data[2]);
      compare_exchange(load_data[3], load_data[4]);
      compare_exchange(load_data[5], load_data[6]);
      compare_exchange(load_data[8], load_data[9]);
      compare_exchange(load_data[10], load_data[11]);
      compare_exchange(load_data[8], load_data[10]);
      compare_exchange(load_data[9], load_data[11]);
      compare_exchange(load_data[9], load_data[10]);
      compare_exchange(load_data[12], load_data[13]);
      compare_exchange(load_data[14], load_data[15]);
      compare_exchange(load_data[12], load_data[14]);
      compare_exchange(load_data[13], load_data[15]);
      compare_exchange(load_data[13], load_data[14]);
      compare_exchange(load_data[8], load_data[12]);
      compare_exchange(load_data[10], load_data[14]);
      compare_exchange(load_data[10], load_data[12]);
      compare_exchange(load_data[9], load_data[13]);
      compare_exchange(load_data[11], load_data[15]);
      compare_exchange(load_data[11], load_data[13]);
      compare_exchange(load_data[9], load_data[10]);
      compare_exchange(load_data[11], load_data[12]);
      compare_exchange(load_data[13], load_data[14]);
      compare_exchange(load_data[0], load_data[8]);
      compare_exchange(load_data[4], load_data[12]);
      compare_exchange(load_data[4], load_data[8]);
      compare_exchange(load_data[2], load_data[10]);
      compare_exchange(load_data[6], load_data[14]);
      compare_exchange(load_data[6], load_data[10]);
      compare_exchange(load_data[2], load_data[4]);
      compare_exchange(load_data[6], load_data[8]);
      compare_exchange(load_data[10], load_data[12]);
      compare_exchange(load_data[1], load_data[9]);
      compare_exchange(load_data[5], load_data[13]);
      compare_exchange(load_data[5], load_data[9]);
      compare_exchange(load_data[3], load_data[11]);
      compare_exchange(load_data[7], load_data[15]);
      compare_exchange(load_data[7], load_data[11]);
      compare_exchange(load_data[3], load_data[5]);
      compare_exchange(load_data[7], load_data[9]);
      compare_exchange(load_data[11], load_data[13]);
      compare_exchange(load_data[1], load_data[2]);
      compare_exchange(load_data[3], load_data[4]);
      compare_exchange(load_data[5], load_data[6]);
      compare_exchange(load_data[7], load_data[8]);
      compare_exchange(load_data[9], load_data[10]);
      compare_exchange(load_data[11], load_data[12]);
      compare_exchange(load_data[13], load_data[14]);
      compare_exchange(load_data[16], load_data[17]);
      compare_exchange(load_data[18], load_data[19]);
      compare_exchange(load_data[16], load_data[18]);
      compare_exchange(load_data[17], load_data[19]);
      compare_exchange(load_data[17], load_data[18]);
      compare_exchange(load_data[20], load_data[21]);
      compare_exchange(load_data[22], load_data[23]);
      compare_exchange(load_data[20], load_data[22]);
      compare_exchange(load_data[21], load_data[23]);
      compare_exchange(load_data[21], load_data[22]);
      compare_exchange(load_data[16], load_data[20]);
      compare_exchange(load_data[18], load_data[22]);
      compare_exchange(load_data[18], load_data[20]);
      compare_exchange(load_data[17], load_data[21]);
      compare_exchange(load_data[19], load_data[23]);
      compare_exchange(load_data[19], load_data[21]);
      compare_exchange(load_data[17], load_data[18]);
      compare_exchange(load_data[19], load_data[20]);
      compare_exchange(load_data[21], load_data[22]);
      compare_exchange(load_data[20], load_data[24]);
      compare_exchange(load_data[22], load_data[24]);
      compare_exchange(load_data[21], load_data[24]);
      compare_exchange(load_data[23], load_data[24]);
      compare_exchange(load_data[16], load_data[20]);
      compare_exchange(load_data[18], load_data[22]);
      compare_exchange(load_data[18], load_data[20]);
      compare_exchange(load_data[17], load_data[21]);
      compare_exchange(load_data[19], load_data[23]);
      compare_exchange(load_data[19], load_data[21]);
      compare_exchange(load_data[17], load_data[18]);
      compare_exchange(load_data[19], load_data[20]);
      compare_exchange(load_data[21], load_data[22]);
      compare_exchange(load_data[0], load_data[16]);
      compare_exchange(load_data[8], load_data[24]);
      compare_exchange(load_data[8], load_data[16]);
      compare_exchange(load_data[4], load_data[20]);
      compare_exchange(load_data[12], load_data[20]);
      compare_exchange(load_data[12], load_data[16]);
      compare_exchange(load_data[2], load_data[18]);
      compare_exchange(load_data[10], load_data[18]);
      compare_exchange(load_data[6], load_data[22]);
      compare_exchange(load_data[6], load_data[10]);
      compare_exchange(load_data[10], load_data[12]);
      compare_exchange(load_data[1], load_data[17]);
      compare_exchange(load_data[9], load_data[17]);
      compare_exchange(load_data[5], load_data[21]);
      compare_exchange(load_data[13], load_data[21]);
      compare_exchange(load_data[13], load_data[17]);
      compare_exchange(load_data[3], load_data[19]);
      compare_exchange(load_data[11], load_data[19]);
      compare_exchange(load_data[7], load_data[23]);
      compare_exchange(load_data[7], load_data[11]);
      compare_exchange(load_data[11], load_data[13]);
      compare_exchange(load_data[11], load_data[12]);

      int wr_offset = row*imgWidth*3 + col;
      __m256i results = load_data[12];
      _mm256_storeu_si256((__m256i*)(imgDst + wr_offset), results);
     }
   }
}
