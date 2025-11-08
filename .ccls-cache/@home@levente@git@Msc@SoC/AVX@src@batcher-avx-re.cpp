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

  __m256i load_data1, load_data2;
  __m256i max_values[16] = {};
  __m256i min_values[16] = {};

  for (int32_t row=0; row<imgHeight; row++)
  {
    for(int32_t col=0; col<imgWidthF; col = col+32)
    {
      //menjek végig egy egész soron és ami értéket lehet számoljak ott ki?

    }

  }
}
