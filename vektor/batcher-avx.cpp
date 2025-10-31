#include "batcher.h"
#include <stdio.h>
#include "stdbool.h"
#include <cstring>
#include <emmintrin.h>
#include <nmmintrin.h>
#include <immintrin.h>
 
void media_filter_vector(int imgHeight, int imgWidth, int imgWidthF,
			   uint8_t *imgSrcExt, uint8_t *imgDst)
{
  __m256i load_data1_1, load_data1_2, max_values_1, min_values_1;
  __m256i load_data2_1, load_data2_2, max_values_2, min_values_2;


  __m256i load_data1, load_data2;
  __m256i max_values[16] = {};
  __m256i min_values[16] = {};

  uint32_t index = 0;

  //min_values[0] = 0
  //max_values[0] = 1
  //min_values[1] = 2
  //max_values[1] = 3
#pragma omp parallel for
	for (int32_t row=0; row<imgHeight; row++)
	{
		int32_t wr_base = row*imgWidth*3;
		int32_t rd_base = row*imgWidthF*3;

		for (int32_t col=0; col<(imgWidth*3); col=col+32)
    {

      for(int fy=0; fy<5; fy++)
      {
        for(int fx=0; fx<5; fx++)
        {



  load_data1 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index));
  load_data2 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+3));
  
  min_values[0] = _mm256_min_epu8(load_data1, load_data2);
  max_values[0] = _mm256_max_epu8(load_data1, load_data2); //0-1 

  index = index + 2*3;

  load_data1 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index));
  load_data2 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+3)); 

  min_values[1] = _mm256_min_epu8(load_data1, load_data2);
  max_values[1] = _mm256_max_epu8(load_data1, load_data2); //2-3

  min_values[0] = _mm256_min_epu8(min_values[0], min_values[1]);
  min_values[1] = _mm256_max_epu8(min_values[0], min_values[1]); //0-2
  
  max_values[0] = _mm256_min_epu8(max_values[0], max_values[1]); 
  max_values[1] = _mm256_max_epu8(max_values[0], max_values[1]); //1-3

  max_values[0] = _mm256_min_epu8(max_values[0], min_values[1]);
  min_values[1] = _mm256_max_epu8(max_values[0], min_values[1]); //1-2

//  
  
  index = index + 4*3; //tuti hogy nem így kell, hanem fx fy-al megindexelni

  load_data1 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index));
  load_data2 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+3));
  
  min_values[2] = _mm256_min_epu8(load_data1, load_data2);
  max_values[2] = _mm256_max_epu8(load_data1, load_data2); //4-5 

  index = index + 2*3;

  load_data1 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index));
  load_data2 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+3)); 

  min_values[3] = _mm256_min_epu8(load_data1, load_data2);
  max_values[3] = _mm256_max_epu8(load_data1, load_data2); //6-7

  min_values[2] = _mm256_min_epu8(min_values[0], min_values[1]);
  min_values[3] = _mm256_max_epu8(min_values[0], min_values[1]); //4-6
  
  max_values[2] = _mm256_min_epu8(max_values[0], max_values[1]); 
  max_values[3] = _mm256_max_epu8(max_values[0], max_values[1]); //5-7

  max_values[2] = _mm256_min_epu8(max_values[0], min_values[1]);
  min_values[3] = _mm256_max_epu8(max_values[0], min_values[1]); //5-6

}
