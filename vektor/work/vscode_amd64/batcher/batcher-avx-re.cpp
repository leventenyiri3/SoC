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
  
  __m256i broadcasted_255 = _mm256_set1_epi8(0xFF);

  //min_values[0] = 0
  //max_values[0] = 1
  //min_values[1] = 2
  //max_values[1] = 3


#pragma omp parallel for
	for (int32_t row=0; row<imgHeight; row++)
	{
		int32_t wr_base = row*imgWidth*3;
		int32_t rd_base = row*imgWidthF*3;

		for (int32_t col=0; col<imgWidthF; col=col+32)
    {

      for(int fy=0; fy<5; fy++)
      {

          int32_t rd_offset = ((row+fy)*imgWidthF + (col))*3;
          load_data1 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+rd_offset));
          load_data2 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+3+rd_offset));
          
          min_values[0] = _mm256_min_epu8(load_data1, load_data2);
          max_values[0] = _mm256_max_epu8(load_data1, load_data2); //0-1

          index = index + 3*2;

          load_data1 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+rd_offset));
          load_data2 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+3+rd_offset));

          min_values[1] = _mm256_min_epu8(load_data1, load_data2);
          max_values[1] = _mm256_max_epu8(load_data1, load_data2); //2-3
      
          min_values[0] = _mm256_min_epu8(min_values[0], min_values[1]);
          min_values[1] = _mm256_max_epu8(min_values[0], min_values[1]); //0-2
          
          max_values[0] = _mm256_min_epu8(max_values[0], max_values[1]); 
          max_values[1] = _mm256_max_epu8(max_values[0], max_values[1]); //1-3

          max_values[0] = _mm256_min_epu8(max_values[0], min_values[1]);
          min_values[1] = _mm256_max_epu8(max_values[0], min_values[1]); //1-2

  //min_values[2] = 4
  //max_values[2] = 5
  //min_values[3] = 6
  //max_values[3] = 7

          index = index + 3;
          
          load_data1 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+rd_offset));
          fy = fy + 1;
          rd_offset = ((row+fy)*imgWidthF + (col))*3;

          index = 0; //ablakban a következő sor első elemével hasonlítani össze
          load_data2 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+rd_offset));
          
          min_values[2] = _mm256_min_epu8(load_data1, load_data2);
          max_values[2] = _mm256_max_epu8(load_data1, load_data2); //4-5


          index = index + 3;
          load_data1 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+rd_offset));
          load_data2 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+3+rd_offset));

          min_values[3] = _mm256_min_epu8(load_data1, load_data2);
          max_values[3] = _mm256_max_epu8(load_data1, load_data2); //6-7
          
          min_values[2] = _mm256_min_epu8(min_values[0], min_values[1]);
          min_values[3] = _mm256_max_epu8(min_values[0], min_values[1]); //4-6
          
          max_values[2] = _mm256_min_epu8(max_values[0], max_values[1]); 
          max_values[3] = _mm256_max_epu8(max_values[0], max_values[1]); //5-7

          max_values[2] = _mm256_min_epu8(max_values[0], min_values[1]);
          min_values[3] = _mm256_max_epu8(max_values[0], min_values[1]); //6-7


/************************************* COMPARE8 **********************************/

  //min_values[0] = 0
  //max_values[0] = 1
  //min_values[1] = 2
  //max_values[1] = 3
  //min_values[2] = 4
  //max_values[2] = 5
  //min_values[3] = 6
  //max_values[3] = 7
        
  uint32_t compare8 = 0;


          min_values[compare8+0] = _mm256_min_epu8(min_values[compare8+0], min_values[compare8+2]);
          min_values[compare8+2] = _mm256_max_epu8(min_values[compare8+0], min_values[compare8+2]); //0-4

          min_values[compare8+1] = _mm256_min_epu8(min_values[compare8+1], min_values[compare8+3]);
          min_values[compare8+3] = _mm256_max_epu8(min_values[compare8+1], min_values[compare8+3]); //2-6

          min_values[compare8+1] = _mm256_min_epu8(min_values[compare8+1], min_values[compare8+2]);
          min_values[compare8+2] = _mm256_max_epu8(min_values[compare8+1], min_values[compare8+2]); //2-4

          max_values[compare8+0] = _mm256_min_epu8(max_values[compare8+0], max_values[compare8+2]);
          max_values[compare8+2] = _mm256_max_epu8(max_values[compare8+0], max_values[compare8+2]); //1-5

          max_values[compare8+1] = _mm256_min_epu8(max_values[compare8+1], max_values[compare8+3]);  
          max_values[compare8+3] = _mm256_max_epu8(max_values[compare8+1], max_values[compare8+3]); //3-7
          
          max_values[compare8+1] = _mm256_min_epu8(max_values[compare8+1], max_values[compare8+2]);  
          max_values[compare8+2] = _mm256_max_epu8(max_values[compare8+1], max_values[compare8+2]); //3-5

          max_values[compare8+0] = _mm256_min_epu8(max_values[compare8+0], min_values[compare8+1]);
          min_values[compare8+1] = _mm256_max_epu8(max_values[compare8+0], min_values[compare8+1]); //1-2

          max_values[compare8+1] = _mm256_min_epu8(max_values[compare8+1], min_values[compare8+2]);  
          min_values[compare8+2] = _mm256_max_epu8(max_values[compare8+1], min_values[compare8+2]); //3-4 
           
          max_values[compare8+2] = _mm256_min_epu8(max_values[compare8+2], min_values[compare8+3]); 
          min_values[compare8+3] = _mm256_max_epu8(max_values[compare8+2], min_values[compare8+3]); //5-6

          compare8 = compare8 + 4;

// COMPARE4

  //min_values[4] = 8
  //max_values[4] = 9
  //min_values[5] = 10
  //max_values[5] = 11
          index = index + 3;

          load_data1 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+rd_offset));
          load_data2 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+3+rd_offset));

          min_values[4] = _mm256_min_epu8(load_data1, load_data2);
          max_values[4] = _mm256_max_epu8(load_data1, load_data2); //8-9
        
          fy = fy + 1;
          index = 0;
          rd_offset = ((row+fy)*imgWidthF + (col))*3;

          load_data1 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+rd_offset));
          load_data2 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+3+rd_offset));
          
          min_values[5] = _mm256_min_epu8(load_data1, load_data2);
          max_values[5] = _mm256_max_epu8(load_data1, load_data2); //10-11
          
          min_values[4] = _mm256_min_epu8(min_values[4], min_values[5]);
          min_values[5] = _mm256_max_epu8(min_values[4], min_values[5]); //8-10
          
          max_values[4] = _mm256_min_epu8(max_values[4], max_values[5]); 
          max_values[5] = _mm256_max_epu8(max_values[4], max_values[5]); //9-11

          max_values[4] = _mm256_min_epu8(max_values[4], min_values[5]);
          min_values[5] = _mm256_max_epu8(max_values[4], min_values[5]); //9-10
       

// COMPARE4
          
  //min_values[6] = 12
  //max_values[6] = 13
  //min_values[7] = 14
  //max_values[7] = 15
          index = index + 2*3;
          
          load_data1 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+rd_offset));
          load_data2 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+3+rd_offset));

          min_values[6] = _mm256_min_epu8(load_data1, load_data2);
          max_values[6] = _mm256_max_epu8(load_data1, load_data2); //12-13
        
          index = index + 3;
          load_data1 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+rd_offset));

          fy = fy + 1;
          index = 0;
          rd_offset = ((row+fy)*imgWidthF + (col))*3;
          load_data2 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+3+rd_offset));

          min_values[7] = _mm256_min_epu8(load_data1, load_data2);
          max_values[7] = _mm256_max_epu8(load_data1, load_data2); //14-15
          
          min_values[6] = _mm256_min_epu8(min_values[6], min_values[7]);
          min_values[7] = _mm256_max_epu8(min_values[6], min_values[7]); //12-14
          
          max_values[6] = _mm256_min_epu8(max_values[6], max_values[7]); 
          max_values[7] = _mm256_max_epu8(max_values[6], max_values[7]); //13-15

          max_values[6] = _mm256_min_epu8(max_values[6], min_values[7]);
          min_values[7] = _mm256_max_epu8(max_values[6], min_values[7]); //13-14


/************************************* COMPARE8 **********************************/

          min_values[compare8+0] = _mm256_min_epu8(min_values[compare8+0], min_values[compare8+2]);
          min_values[compare8+2] = _mm256_max_epu8(min_values[compare8+0], min_values[compare8+2]); 

          min_values[compare8+1] = _mm256_min_epu8(min_values[compare8+1], min_values[compare8+3]);
          min_values[compare8+3] = _mm256_max_epu8(min_values[compare8+1], min_values[compare8+3]); 

          min_values[compare8+1] = _mm256_min_epu8(min_values[compare8+1], min_values[compare8+2]);
          min_values[compare8+2] = _mm256_max_epu8(min_values[compare8+1], min_values[compare8+2]); 

          max_values[compare8+0] = _mm256_min_epu8(max_values[compare8+0], max_values[compare8+2]);
          max_values[compare8+2] = _mm256_max_epu8(max_values[compare8+0], max_values[compare8+2]); 

          max_values[compare8+1] = _mm256_min_epu8(max_values[compare8+1], max_values[compare8+3]); 
          max_values[compare8+3] = _mm256_max_epu8(max_values[compare8+1], max_values[compare8+3]); 
          
          max_values[compare8+1] = _mm256_min_epu8(max_values[compare8+1], max_values[compare8+2]); 
          max_values[compare8+2] = _mm256_max_epu8(max_values[compare8+1], max_values[compare8+2]); 

          max_values[compare8+0] = _mm256_min_epu8(max_values[compare8+0], min_values[compare8+1]);
          min_values[compare8+1] = _mm256_max_epu8(max_values[compare8+0], min_values[compare8+1]); 

          max_values[compare8+1] = _mm256_min_epu8(max_values[compare8+1], min_values[compare8+2]); 
          min_values[compare8+2] = _mm256_max_epu8(max_values[compare8+1], min_values[compare8+2]);  
           
          max_values[compare8+2] = _mm256_min_epu8(max_values[compare8+2], min_values[compare8+3]); 
          min_values[compare8+3] = _mm256_max_epu8(max_values[compare8+2], min_values[compare8+3]); 

          compare8 = compare8 + 4;


/************************************* COMPARE16 **********************************/

  //min_values[0] = 0
  //max_values[0] = 1
  //min_values[1] = 2
  //max_values[1] = 3
  //min_values[2] = 4
  //max_values[2] = 5
  //min_values[3] = 6
  //max_values[3] = 7
  //min_values[4] = 8
  //max_values[4] = 9
  //min_values[5] = 10
  //max_values[5] = 11
  //min_values[6] = 12
  //max_values[6] = 13
  //min_values[7] = 14
  //max_values[7] = 15
  uint32_t compare16 = 0;
          
          min_values[compare16+0] = _mm256_min_epu8(min_values[compare16+0], min_values[compare16+4]);
          min_values[compare16+4] = _mm256_max_epu8(min_values[compare16+0], min_values[compare16+4]); 

          min_values[compare16+2] = _mm256_min_epu8(min_values[compare16+2], min_values[compare16+6]);
          min_values[compare16+6] = _mm256_max_epu8(min_values[compare16+2], min_values[compare16+6]); 

          min_values[compare16+2] = _mm256_min_epu8(min_values[compare16+2], min_values[compare16+4]);
          min_values[compare16+4] = _mm256_max_epu8(min_values[compare16+2], min_values[compare16+4]); 

          min_values[compare16+1] = _mm256_min_epu8(min_values[compare16+1], min_values[compare16+5]);
          min_values[compare16+5] = _mm256_max_epu8(min_values[compare16+1], min_values[compare16+5]); 

          min_values[compare16+3] = _mm256_min_epu8(min_values[compare16+3], min_values[compare16+7]); 
          min_values[compare16+7] = _mm256_max_epu8(min_values[compare16+3], min_values[compare16+7]); 
          
          min_values[compare16+3] = _mm256_min_epu8(min_values[compare16+3], min_values[compare16+5]); 
          min_values[compare16+5] = _mm256_max_epu8(min_values[compare16+3], min_values[compare16+5]); 

          min_values[compare16+1] = _mm256_min_epu8(min_values[compare16+1], min_values[compare16+2]);
          min_values[compare16+2] = _mm256_max_epu8(min_values[compare16+1], min_values[compare16+2]); 

          min_values[compare16+3] = _mm256_min_epu8(min_values[compare16+3], min_values[compare16+4]); 
          min_values[compare16+4] = _mm256_max_epu8(min_values[compare16+3], min_values[compare16+4]);  
           
          min_values[compare16+5] = _mm256_min_epu8(min_values[compare16+5], min_values[compare16+6]); 
          min_values[compare16+6] = _mm256_max_epu8(min_values[compare16+5], min_values[compare16+6]); 


          max_values[compare16+0] = _mm256_min_epu8(max_values[compare16+0], max_values[compare16+4]);
          max_values[compare16+4] = _mm256_max_epu8(max_values[compare16+0], max_values[compare16+4]); 

          max_values[compare16+2] = _mm256_min_epu8(max_values[compare16+2], max_values[compare16+6]);
          max_values[compare16+6] = _mm256_max_epu8(max_values[compare16+2], max_values[compare16+6]); 

          max_values[compare16+2] = _mm256_min_epu8(max_values[compare16+2], max_values[compare16+4]);
          max_values[compare16+4] = _mm256_max_epu8(max_values[compare16+2], max_values[compare16+4]); 

          max_values[compare16+1] = _mm256_min_epu8(max_values[compare16+1], max_values[compare16+5]);
          max_values[compare16+5] = _mm256_max_epu8(max_values[compare16+1], max_values[compare16+5]); 

          max_values[compare16+3] = _mm256_min_epu8(max_values[compare16+3], max_values[compare16+7]); 
          max_values[compare16+7] = _mm256_max_epu8(max_values[compare16+3], max_values[compare16+7]); 
          
          max_values[compare16+3] = _mm256_min_epu8(max_values[compare16+3], max_values[compare16+5]); 
          max_values[compare16+5] = _mm256_max_epu8(max_values[compare16+3], max_values[compare16+5]); 

          max_values[compare16+1] = _mm256_min_epu8(max_values[compare16+1], max_values[compare16+2]);
          max_values[compare16+2] = _mm256_max_epu8(max_values[compare16+1], max_values[compare16+2]); 

          max_values[compare16+3] = _mm256_min_epu8(max_values[compare16+3], max_values[compare16+4]); 
          max_values[compare16+4] = _mm256_max_epu8(max_values[compare16+3], max_values[compare16+4]);  
           
          max_values[compare16+5] = _mm256_min_epu8(max_values[compare16+5], max_values[compare16+6]); 
          max_values[compare16+6] = _mm256_max_epu8(max_values[compare16+5], max_values[compare16+6]); 

        
          max_values[compare16+0] = _mm256_min_epu8(max_values[compare16+0], min_values[compare16+1]);
          min_values[compare16+1] = _mm256_max_epu8(max_values[compare16+0], min_values[compare16+1]);

          max_values[compare16+1] = _mm256_min_epu8(max_values[compare16+1], min_values[compare16+2]);
          min_values[compare16+2] = _mm256_max_epu8(max_values[compare16+1], min_values[compare16+2]);
          
          max_values[compare16+2] = _mm256_min_epu8(max_values[compare16+2], min_values[compare16+3]);
          min_values[compare16+3] = _mm256_max_epu8(max_values[compare16+2], min_values[compare16+3]);

          max_values[compare16+3] = _mm256_min_epu8(max_values[compare16+3], min_values[compare16+4]);
          min_values[compare16+4] = _mm256_max_epu8(max_values[compare16+3], min_values[compare16+4]);

          max_values[compare16+4] = _mm256_min_epu8(max_values[compare16+4], min_values[compare16+5]);
          min_values[compare16+5] = _mm256_max_epu8(max_values[compare16+4], min_values[compare16+5]);
          
          max_values[compare16+5] = _mm256_min_epu8(max_values[compare16+5], min_values[compare16+6]);
          min_values[compare16+6] = _mm256_max_epu8(max_values[compare16+5], min_values[compare16+6]);

          max_values[compare16+6] = _mm256_min_epu8(max_values[compare16+6], min_values[compare16+7]);
          min_values[compare16+7] = _mm256_max_epu8(max_values[compare16+6], min_values[compare16+7]);

          compare16 = compare16 + 8;
//COMPARE4
  //min_values[8] = 16
  //max_values[8] = 17
  //min_values[9] = 18
  //max_values[9] = 19
        
          index = index + 3;
          load_data1 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+rd_offset));
          load_data2 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+3+rd_offset));

          min_values[8] = _mm256_min_epu8(load_data1, load_data2);
          max_values[8] = _mm256_max_epu8(load_data1, load_data2);  //16-17

          index = index + 2*3;
          load_data1 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+rd_offset));
          load_data2 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+3+rd_offset));

          min_values[9] = _mm256_min_epu8(load_data1, load_data2);
          max_values[9] = _mm256_max_epu8(load_data1, load_data2);  //18-19
          
          min_values[8] = _mm256_min_epu8(min_values[8], min_values[9]);
          min_values[9] = _mm256_max_epu8(min_values[8], min_values[9]);
          
          max_values[8] = _mm256_min_epu8(max_values[8], max_values[9]);
          max_values[9] = _mm256_max_epu8(max_values[8], max_values[9]);

          max_values[8] = _mm256_min_epu8(max_values[8], min_values[9]);
          min_values[9] = _mm256_max_epu8(max_values[8], min_values[9]);

//COMPARE4
  //min_values[10] = 20
  //max_values[10] = 21
  //min_values[11] = 22
  //max_values[11] = 23
        
          fy = fy + 1;
          index = 0;
          rd_offset = ((row+fy)*imgWidthF + (col))*3;
          load_data1 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+rd_offset));
          load_data2 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+3+rd_offset));

          min_values[10] = _mm256_min_epu8(load_data1, load_data2);
          max_values[10] = _mm256_max_epu8(load_data1, load_data2);  //20-21

          index = index + 2*3;
          load_data1 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+rd_offset));
          load_data2 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+3+rd_offset));

          min_values[11] = _mm256_min_epu8(load_data1, load_data2);
          max_values[11] = _mm256_max_epu8(load_data1, load_data2);  //22-23
          
          min_values[10] = _mm256_min_epu8(min_values[10], min_values[11]);
          min_values[11] = _mm256_max_epu8(min_values[10], min_values[11]);
          
          max_values[10] = _mm256_min_epu8(max_values[10], max_values[11]);
          max_values[11] = _mm256_max_epu8(max_values[10], max_values[11]);

          max_values[10] = _mm256_min_epu8(max_values[10], min_values[11]);
          min_values[11] = _mm256_max_epu8(max_values[10], min_values[11]);

/************************************* COMPARE8 **********************************/

          min_values[compare8+0] = _mm256_min_epu8(min_values[compare8+0], min_values[compare8+2]);
          min_values[compare8+2] = _mm256_max_epu8(min_values[compare8+0], min_values[compare8+2]); 

          min_values[compare8+1] = _mm256_min_epu8(min_values[compare8+1], min_values[compare8+3]);
          min_values[compare8+3] = _mm256_max_epu8(min_values[compare8+1], min_values[compare8+3]); 

          min_values[compare8+1] = _mm256_min_epu8(min_values[compare8+1], min_values[compare8+2]);
          min_values[compare8+2] = _mm256_max_epu8(min_values[compare8+1], min_values[compare8+2]); 

          max_values[compare8+0] = _mm256_min_epu8(max_values[compare8+0], max_values[compare8+2]);
          max_values[compare8+2] = _mm256_max_epu8(max_values[compare8+0], max_values[compare8+2]); 

          max_values[compare8+1] = _mm256_min_epu8(max_values[compare8+1], max_values[compare8+3]); 
          max_values[compare8+3] = _mm256_max_epu8(max_values[compare8+1], max_values[compare8+3]); 
          
          max_values[compare8+1] = _mm256_min_epu8(max_values[compare8+1], max_values[compare8+2]); 
          max_values[compare8+2] = _mm256_max_epu8(max_values[compare8+1], max_values[compare8+2]); 

          max_values[compare8+0] = _mm256_min_epu8(max_values[compare8+0], min_values[compare8+1]);
          min_values[compare8+1] = _mm256_max_epu8(max_values[compare8+0], min_values[compare8+1]); 

          max_values[compare8+1] = _mm256_min_epu8(max_values[compare8+1], min_values[compare8+2]); 
          min_values[compare8+2] = _mm256_max_epu8(max_values[compare8+1], min_values[compare8+2]);  
           
          max_values[compare8+2] = _mm256_min_epu8(max_values[compare8+2], min_values[compare8+3]); 
          min_values[compare8+3] = _mm256_max_epu8(max_values[compare8+2], min_values[compare8+3]); 
          
          compare8 = compare8 + 4;
        
//COMPARE4
  //min_values[12] = 24
  //max_values[12] = 25
  //min_values[13] = 26
  //max_values[13] = 27

          index = index + 3;
          load_data1 = _mm256_lddqu_si256((__m256i *)(imgSrcExt+index+rd_offset));

          min_values[12] = _mm256_min_epu8(load_data1, broadcasted_255);
          max_values[12] = broadcasted_255;
          min_values[13] = broadcasted_255;
          max_values[13] = broadcasted_255;
          min_values[14] = broadcasted_255;
          max_values[14] = broadcasted_255;
          min_values[15] = broadcasted_255;
          max_values[15] = broadcasted_255; //31
          

/************************************* COMPARE8 **********************************/

          min_values[compare8+0] = _mm256_min_epu8(min_values[compare8+0], min_values[compare8+2]);
          min_values[compare8+2] = _mm256_max_epu8(min_values[compare8+0], min_values[compare8+2]); 

          min_values[compare8+1] = _mm256_min_epu8(min_values[compare8+1], min_values[compare8+3]);
          min_values[compare8+3] = _mm256_max_epu8(min_values[compare8+1], min_values[compare8+3]); 

          min_values[compare8+1] = _mm256_min_epu8(min_values[compare8+1], min_values[compare8+2]);
          min_values[compare8+2] = _mm256_max_epu8(min_values[compare8+1], min_values[compare8+2]); 

          max_values[compare8+0] = _mm256_min_epu8(max_values[compare8+0], max_values[compare8+2]);
          max_values[compare8+2] = _mm256_max_epu8(max_values[compare8+0], max_values[compare8+2]); 

          max_values[compare8+1] = _mm256_min_epu8(max_values[compare8+1], max_values[compare8+3]); 
          max_values[compare8+3] = _mm256_max_epu8(max_values[compare8+1], max_values[compare8+3]); 
          
          max_values[compare8+1] = _mm256_min_epu8(max_values[compare8+1], max_values[compare8+2]); 
          max_values[compare8+2] = _mm256_max_epu8(max_values[compare8+1], max_values[compare8+2]); 

          max_values[compare8+0] = _mm256_min_epu8(max_values[compare8+0], min_values[compare8+1]);
          min_values[compare8+1] = _mm256_max_epu8(max_values[compare8+0], min_values[compare8+1]); 

          max_values[compare8+1] = _mm256_min_epu8(max_values[compare8+1], min_values[compare8+2]); 
          min_values[compare8+2] = _mm256_max_epu8(max_values[compare8+1], min_values[compare8+2]);  
           
          max_values[compare8+2] = _mm256_min_epu8(max_values[compare8+2], min_values[compare8+3]); 
          min_values[compare8+3] = _mm256_max_epu8(max_values[compare8+2], min_values[compare8+3]); 
          
          compare8 = compare8 + 4;

          
/************************************* COMPARE16 **********************************/

  //min_values[8] = 16
  //max_values[8] = 17
  //min_values[9] = 18
  //max_values[9] = 19
  //min_values[10] = 20
  //max_values[10] = 21
  //min_values[11] = 22
  //max_values[11] = 23
  //min_values[12] = 24
  //max_values[12] = 25
  //min_values[13] = 26
  //max_values[13] = 27
  //min_values[14] = 28
  //max_values[14] = 29
  //min_values[15] = 30
  //max_values[15] = 31
          
          min_values[compare16+0] = _mm256_min_epu8(min_values[compare16+0], min_values[compare16+4]);
          min_values[compare16+4] = _mm256_max_epu8(min_values[compare16+0], min_values[compare16+4]); 

          min_values[compare16+2] = _mm256_min_epu8(min_values[compare16+2], min_values[compare16+6]);
          min_values[compare16+6] = _mm256_max_epu8(min_values[compare16+2], min_values[compare16+6]); 

          min_values[compare16+2] = _mm256_min_epu8(min_values[compare16+2], min_values[compare16+4]);
          min_values[compare16+4] = _mm256_max_epu8(min_values[compare16+2], min_values[compare16+4]); 

          min_values[compare16+1] = _mm256_min_epu8(min_values[compare16+1], min_values[compare16+5]);
          min_values[compare16+5] = _mm256_max_epu8(min_values[compare16+1], min_values[compare16+5]); 

          min_values[compare16+3] = _mm256_min_epu8(min_values[compare16+3], min_values[compare16+7]); 
          min_values[compare16+7] = _mm256_max_epu8(min_values[compare16+3], min_values[compare16+7]); 
          
          min_values[compare16+3] = _mm256_min_epu8(min_values[compare16+3], min_values[compare16+5]); 
          min_values[compare16+5] = _mm256_max_epu8(min_values[compare16+3], min_values[compare16+5]); 

          min_values[compare16+1] = _mm256_min_epu8(min_values[compare16+1], min_values[compare16+2]);
          min_values[compare16+2] = _mm256_max_epu8(min_values[compare16+1], min_values[compare16+2]); 

          min_values[compare16+3] = _mm256_min_epu8(min_values[compare16+3], min_values[compare16+4]); 
          min_values[compare16+4] = _mm256_max_epu8(min_values[compare16+3], min_values[compare16+4]);  
           
          min_values[compare16+5] = _mm256_min_epu8(min_values[compare16+5], min_values[compare16+6]); 
          min_values[compare16+6] = _mm256_max_epu8(min_values[compare16+5], min_values[compare16+6]); 


          max_values[compare16+0] = _mm256_min_epu8(max_values[compare16+0], max_values[compare16+4]);
          max_values[compare16+4] = _mm256_max_epu8(max_values[compare16+0], max_values[compare16+4]); 

          max_values[compare16+2] = _mm256_min_epu8(max_values[compare16+2], max_values[compare16+6]);
          max_values[compare16+6] = _mm256_max_epu8(max_values[compare16+2], max_values[compare16+6]); 

          max_values[compare16+2] = _mm256_min_epu8(max_values[compare16+2], max_values[compare16+4]);
          max_values[compare16+4] = _mm256_max_epu8(max_values[compare16+2], max_values[compare16+4]); 

          max_values[compare16+1] = _mm256_min_epu8(max_values[compare16+1], max_values[compare16+5]);
          max_values[compare16+5] = _mm256_max_epu8(max_values[compare16+1], max_values[compare16+5]); 

          max_values[compare16+3] = _mm256_min_epu8(max_values[compare16+3], max_values[compare16+7]); 
          max_values[compare16+7] = _mm256_max_epu8(max_values[compare16+3], max_values[compare16+7]); 
          
          max_values[compare16+3] = _mm256_min_epu8(max_values[compare16+3], max_values[compare16+5]); 
          max_values[compare16+5] = _mm256_max_epu8(max_values[compare16+3], max_values[compare16+5]); 

          max_values[compare16+1] = _mm256_min_epu8(max_values[compare16+1], max_values[compare16+2]);
          max_values[compare16+2] = _mm256_max_epu8(max_values[compare16+1], max_values[compare16+2]); 

          max_values[compare16+3] = _mm256_min_epu8(max_values[compare16+3], max_values[compare16+4]); 
          max_values[compare16+4] = _mm256_max_epu8(max_values[compare16+3], max_values[compare16+4]);  
           
          max_values[compare16+5] = _mm256_min_epu8(max_values[compare16+5], max_values[compare16+6]); 
          max_values[compare16+6] = _mm256_max_epu8(max_values[compare16+5], max_values[compare16+6]); 

        
          max_values[compare16+0] = _mm256_min_epu8(max_values[compare16+0], min_values[compare16+1]);
          min_values[compare16+1] = _mm256_max_epu8(max_values[compare16+0], min_values[compare16+1]);

          max_values[compare16+1] = _mm256_min_epu8(max_values[compare16+1], min_values[compare16+2]);
          min_values[compare16+2] = _mm256_max_epu8(max_values[compare16+1], min_values[compare16+2]);
          
          max_values[compare16+2] = _mm256_min_epu8(max_values[compare16+2], min_values[compare16+3]);
          min_values[compare16+3] = _mm256_max_epu8(max_values[compare16+2], min_values[compare16+3]);

          max_values[compare16+3] = _mm256_min_epu8(max_values[compare16+3], min_values[compare16+4]);
          min_values[compare16+4] = _mm256_max_epu8(max_values[compare16+3], min_values[compare16+4]);

          max_values[compare16+4] = _mm256_min_epu8(max_values[compare16+4], min_values[compare16+5]);
          min_values[compare16+5] = _mm256_max_epu8(max_values[compare16+4], min_values[compare16+5]);
          
          max_values[compare16+5] = _mm256_min_epu8(max_values[compare16+5], min_values[compare16+6]);
          min_values[compare16+6] = _mm256_max_epu8(max_values[compare16+5], min_values[compare16+6]);

          max_values[compare16+6] = _mm256_min_epu8(max_values[compare16+6], min_values[compare16+7]);
          min_values[compare16+7] = _mm256_max_epu8(max_values[compare16+6], min_values[compare16+7]);

          compare16 = compare16 + 8;

/************************************* COMPARE32 **********************************/

         min_values[0]   =  _mm256_min_epu8(min_values[0], min_values[8]);
         min_values[8]   =  _mm256_max_epu8(min_values[0], min_values[8]);
         
         min_values[4]   =  _mm256_min_epu8(min_values[4], min_values[12]);
         min_values[12]  =  _mm256_max_epu8(min_values[4], min_values[12]);
         
         min_values[4]   =  _mm256_min_epu8(min_values[4], min_values[8]);
         min_values[8]   =  _mm256_max_epu8(min_values[4], min_values[8]);

         min_values[2]   =  _mm256_min_epu8(min_values[2], min_values[10]);
         min_values[10]  =  _mm256_max_epu8(min_values[2], min_values[10]);
         
         min_values[6]   =  _mm256_min_epu8(min_values[6], min_values[14]);
         min_values[14]  =  _mm256_max_epu8(min_values[6], min_values[14]);
         
         min_values[6]   =  _mm256_min_epu8(min_values[6], min_values[10]);
         min_values[10]  =  _mm256_max_epu8(min_values[6], min_values[10]);
         
         min_values[2]   =  _mm256_min_epu8(min_values[2], min_values[4]);
         min_values[4]   =  _mm256_max_epu8(min_values[2], min_values[4]);
         
         min_values[6]   =  _mm256_min_epu8(min_values[6], min_values[8]);
         min_values[8]   =  _mm256_max_epu8(min_values[6], min_values[8]);
         
         min_values[10]  =  _mm256_min_epu8(min_values[10], min_values[12]);
         min_values[12]  =  _mm256_max_epu8(min_values[10], min_values[12]);

         min_values[1]   =  _mm256_min_epu8(min_values[1], min_values[9]);
         min_values[9]   =  _mm256_max_epu8(min_values[1], min_values[9]);

         min_values[5]   =  _mm256_min_epu8(min_values[5], min_values[13]);
         min_values[13]  =  _mm256_max_epu8(min_values[5], min_values[13]);

         min_values[4]   =  _mm256_min_epu8(min_values[4], min_values[12]);
         min_values[12]  =  _mm256_max_epu8(min_values[4], min_values[12]);

         min_values[5]   =  _mm256_min_epu8(min_values[5], min_values[9]);
         min_values[9]   =  _mm256_max_epu8(min_values[5], min_values[9]);
        
         min_values[3]   =  _mm256_min_epu8(min_values[3], min_values[11]);
         min_values[11]  =  _mm256_max_epu8(min_values[3], min_values[11]);

         min_values[7]   =  _mm256_min_epu8(min_values[7], min_values[15]);
         min_values[15]  =  _mm256_max_epu8(min_values[7], min_values[15]);

         min_values[7]   =  _mm256_min_epu8(min_values[7], min_values[11]);
         min_values[11]  =  _mm256_max_epu8(min_values[7], min_values[11]);

         min_values[3]   =  _mm256_min_epu8(min_values[3], min_values[5]);
         min_values[5]   =  _mm256_max_epu8(min_values[3], min_values[5]);

         min_values[7]   =  _mm256_min_epu8(min_values[7], min_values[9]);
         min_values[9]   =  _mm256_max_epu8(min_values[7], min_values[9]);

         min_values[11]  =  _mm256_min_epu8(min_values[11], min_values[13]);
         min_values[13]  =  _mm256_max_epu8(min_values[11], min_values[13]);


         min_values[1]   =  _mm256_min_epu8(min_values[1], min_values[2]);
         min_values[2]   =  _mm256_max_epu8(min_values[1], min_values[2]);
        
         min_values[3]   =  _mm256_min_epu8(min_values[3], min_values[4]);
         min_values[4]   =  _mm256_max_epu8(min_values[3], min_values[4]);

         min_values[5]   =  _mm256_min_epu8(min_values[5], min_values[6]);
         min_values[6]   =  _mm256_max_epu8(min_values[5], min_values[6]);

         min_values[7]   =  _mm256_min_epu8(min_values[7], min_values[8]);
         min_values[8]   =  _mm256_max_epu8(min_values[7], min_values[8]);

         min_values[9]   =  _mm256_min_epu8(min_values[9], min_values[10]);
         min_values[10]  =  _mm256_max_epu8(min_values[9], min_values[10]);

         min_values[11]  =  _mm256_min_epu8(min_values[11], min_values[12]);
         min_values[12]  =  _mm256_max_epu8(min_values[11], min_values[12]);
        
         min_values[13]  =  _mm256_min_epu8(min_values[13], min_values[14]);
         min_values[14]  =  _mm256_max_epu8(min_values[13], min_values[14]);
         

         
         max_values[0]   =  _mm256_min_epu8(max_values[0], max_values[8]);
         max_values[8]   =  _mm256_max_epu8(max_values[0], max_values[8]);
         
         max_values[4]   =  _mm256_min_epu8(max_values[4], max_values[12]);
         max_values[12]  =  _mm256_max_epu8(max_values[4], max_values[12]);
         
         max_values[4]   =  _mm256_min_epu8(max_values[4], max_values[8]);
         max_values[8]   =  _mm256_max_epu8(max_values[4], max_values[8]);

         max_values[2]   =  _mm256_min_epu8(max_values[2], max_values[10]);
         max_values[10]  =  _mm256_max_epu8(max_values[2], max_values[10]);
         
         max_values[6]   =  _mm256_min_epu8(max_values[6], max_values[14]);
         max_values[14]  =  _mm256_max_epu8(max_values[6], max_values[14]);
         
         max_values[6]   =  _mm256_min_epu8(max_values[6], max_values[10]);
         max_values[10]  =  _mm256_max_epu8(max_values[6], max_values[10]);
         
         max_values[2]   =  _mm256_min_epu8(max_values[2], max_values[4]);
         max_values[4]   =  _mm256_max_epu8(max_values[2], max_values[4]);
         
         max_values[6]   =  _mm256_min_epu8(max_values[6], max_values[8]);
         max_values[8]   =  _mm256_max_epu8(max_values[6], max_values[8]);
         
         max_values[10]  =  _mm256_min_epu8(max_values[10], max_values[12]);
         max_values[12]  =  _mm256_max_epu8(max_values[10], max_values[12]);

         max_values[1]   =  _mm256_min_epu8(max_values[1], max_values[9]);
         max_values[9]   =  _mm256_max_epu8(max_values[1], max_values[9]);

         max_values[5]   =  _mm256_min_epu8(max_values[5], max_values[13]);
         max_values[13]  =  _mm256_max_epu8(max_values[5], max_values[13]);

         max_values[4]   =  _mm256_min_epu8(max_values[4], max_values[12]);
         max_values[12]  =  _mm256_max_epu8(max_values[4], max_values[12]);

         max_values[5]   =  _mm256_min_epu8(max_values[5], max_values[9]);
         max_values[9]   =  _mm256_max_epu8(max_values[5], max_values[9]);
        
         max_values[3]   =  _mm256_min_epu8(max_values[3], max_values[11]);
         max_values[11]  =  _mm256_max_epu8(max_values[3], max_values[11]);

         max_values[7]   =  _mm256_min_epu8(max_values[7], max_values[15]);
         max_values[15]  =  _mm256_max_epu8(max_values[7], max_values[15]);

         max_values[7]   =  _mm256_min_epu8(max_values[7], max_values[11]);
         max_values[11]  =  _mm256_max_epu8(max_values[7], max_values[11]);

         max_values[3]   =  _mm256_min_epu8(max_values[3], max_values[5]);
         max_values[5]   =  _mm256_max_epu8(max_values[3], max_values[5]);

         max_values[7]   =  _mm256_min_epu8(max_values[7], max_values[9]);
         max_values[9]   =  _mm256_max_epu8(max_values[7], max_values[9]);

         max_values[11]  =  _mm256_min_epu8(max_values[11], max_values[13]);
         max_values[13]  =  _mm256_max_epu8(max_values[11], max_values[13]);


         max_values[1]   =  _mm256_min_epu8(max_values[1], max_values[2]);
         max_values[2]   =  _mm256_max_epu8(max_values[1], max_values[2]);
        
         max_values[3]   =  _mm256_min_epu8(max_values[3], max_values[4]);
         max_values[4]   =  _mm256_max_epu8(max_values[3], max_values[4]);

         max_values[5]   =  _mm256_min_epu8(max_values[5], max_values[6]);
         max_values[6]   =  _mm256_max_epu8(max_values[5], max_values[6]);

         max_values[7]   =  _mm256_min_epu8(max_values[7], max_values[8]);
         max_values[8]   =  _mm256_max_epu8(max_values[7], max_values[8]);

         max_values[9]   =  _mm256_min_epu8(max_values[9], max_values[10]);
         max_values[10]  =  _mm256_max_epu8(max_values[9], max_values[10]);

         max_values[11]  =  _mm256_min_epu8(max_values[11], max_values[12]);
         max_values[12]  =  _mm256_max_epu8(max_values[11], max_values[12]);
        
         max_values[13]  =  _mm256_min_epu8(max_values[13], max_values[14]);
         max_values[14]  =  _mm256_max_epu8(max_values[13], max_values[14]);


         for(int i=0; i<14; i++)
        {

          max_values[i] = _mm256_min_epu8(max_values[i], min_values[i+1]);
          min_values[i+1] = _mm256_max_epu8(max_values[i], min_values[i+1]);

        }


      }


      
			int wr_offset;
			wr_offset = row*imgWidth*3 + col*32;

      __m256i results;
     _mm256_storeu_si256(&results, min_values[6]); 
      for(int i=0; i<15; i++)
      {
        *(imgDst + wr_offset) = _mm256_extract_epi8(results, i); 
      }

    }

  }

}
