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
  __m256i load_data[2] = {};
  __m256i switched_values[25];


  for (int32_t row=0; row<imgHeight; row++)
  {

    for(int32_t col=0; col<imgWidthF*3; col = col + 32)
    {
      uint32_t switched_value_idx = 0;

//      uint32_t fy = 0;

      uint32_t component_offset = 0;

      int32_t rd_offset = ((row)*imgWidthF*3 + col);
      

      while(switched_value_idx  < 24)
      {
        load_data[0] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + component_offset + rd_offset));
         
        if(component_offset + 3 <= 12)
        {
           component_offset += 3;
        }
        else
        {
           component_offset = 0;
           rd_offset += imgWidthF*3;
        }
          
        load_data[1] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + component_offset + rd_offset));


        if(component_offset + 3 <= 12)
        {
           component_offset += 3;
        }
        else
        {
           component_offset = 0;
           rd_offset += imgWidthF*3;
        }

        compare_exchange(load_data[0], load_data[1]);
        switched_values[switched_value_idx] = load_data[0];
        switched_value_idx++;
        switched_values[switched_value_idx] = load_data[1];
        switched_value_idx++;


        if(switched_value_idx % 4 == 0)
        {
          compare_exchange(switched_values[switched_value_idx-4], switched_values[switched_value_idx-2]); // 0-2, 4-6, 8-10, 12-14, 16-18, 20-22
          compare_exchange(switched_values[switched_value_idx-3], switched_values[switched_value_idx-1]); // 1-3, 5-7, 9-11, 13-15, 17-19, 21-23
          compare_exchange(switched_values[switched_value_idx-3], switched_values[switched_value_idx-2]); // 1-2, 4-5, 8-9, 12-13, 16-17, 20-21
        }

        if(switched_value_idx % 8 == 0)
        {
          compare_exchange(switched_values[switched_value_idx-8], switched_values[switched_value_idx-4]); // 0-4, 8-12, 16-20, 24-28
          compare_exchange(switched_values[switched_value_idx-6], switched_values[switched_value_idx-2]); // 2-6, 10-14, 18-22, 26-30
          compare_exchange(switched_values[switched_value_idx-6], switched_values[switched_value_idx-4]); // 2-4, 10-12, 18-20, 26-28

          compare_exchange(switched_values[switched_value_idx-7], switched_values[switched_value_idx-3]); // 1-5, 9-13, 17-21, 25-29
          compare_exchange(switched_values[switched_value_idx-5], switched_values[switched_value_idx-1]); // 3-7, 11-15, 19-23, 27-31
          compare_exchange(switched_values[switched_value_idx-5], switched_values[switched_value_idx-3]); // 3-5, 11-13, 19-21, 27-29

          compare_exchange(switched_values[switched_value_idx-7], switched_values[switched_value_idx-6]); // 1-2, 9-10, 17-18, 25-26
          compare_exchange(switched_values[switched_value_idx-5], switched_values[switched_value_idx-4]); // 3-4, 11-12, 19-20, 27-28
          compare_exchange(switched_values[switched_value_idx-3], switched_values[switched_value_idx-2]); // 5-6, 13-14, 21-22, 29-30
        }
        
        if(switched_value_idx % 16 == 0) //itt majd kell még a ciklus után egy részlegest csinálni-bár nem biztos, meglátjuk
        {
          compare_exchange(switched_values[switched_value_idx-16], switched_values[switched_value_idx-8]); // 0-8
          compare_exchange(switched_values[switched_value_idx-12], switched_values[switched_value_idx-4]); // 4-12
          compare_exchange(switched_values[switched_value_idx-12], switched_values[switched_value_idx-8]); // 4-8
       
          compare_exchange(switched_values[switched_value_idx-14], switched_values[switched_value_idx-6]); // 2-10
          compare_exchange(switched_values[switched_value_idx-10], switched_values[switched_value_idx-2]); // 6-14 
          compare_exchange(switched_values[switched_value_idx-10], switched_values[switched_value_idx-6]); // 6-10 

          compare_exchange(switched_values[switched_value_idx-14], switched_values[switched_value_idx-12]); // 2-4 
          compare_exchange(switched_values[switched_value_idx-10], switched_values[switched_value_idx-8]); // 6-8 
          compare_exchange(switched_values[switched_value_idx-6], switched_values[switched_value_idx-4]); // 10-12 

          compare_exchange(switched_values[switched_value_idx-15], switched_values[switched_value_idx-7]); // 0-9
          compare_exchange(switched_values[switched_value_idx-11], switched_values[switched_value_idx-3]); // 5-13
          compare_exchange(switched_values[switched_value_idx-11], switched_values[switched_value_idx-7]); // 5-9
       
          compare_exchange(switched_values[switched_value_idx-13], switched_values[switched_value_idx-5]); // 3-11
          compare_exchange(switched_values[switched_value_idx-9], switched_values[switched_value_idx-1]); // 7-15 
          compare_exchange(switched_values[switched_value_idx-9], switched_values[switched_value_idx-5]); // 7-11 

          compare_exchange(switched_values[switched_value_idx-13], switched_values[switched_value_idx-11]); // 3-5 
          compare_exchange(switched_values[switched_value_idx-9], switched_values[switched_value_idx-7]); // 7-9 
          compare_exchange(switched_values[switched_value_idx-5], switched_values[switched_value_idx-3]); // 11-13 

          compare_exchange(switched_values[switched_value_idx-15], switched_values[switched_value_idx-14]); // 1-2 
          compare_exchange(switched_values[switched_value_idx-13], switched_values[switched_value_idx-12]); // 3-4 
          compare_exchange(switched_values[switched_value_idx-11], switched_values[switched_value_idx-10]); // 5-6 
          compare_exchange(switched_values[switched_value_idx-9], switched_values[switched_value_idx-8]); // 7-8 
          compare_exchange(switched_values[switched_value_idx-7], switched_values[switched_value_idx-6]); // 9-10 
          compare_exchange(switched_values[switched_value_idx-5], switched_values[switched_value_idx-4]); // 11-12 
          compare_exchange(switched_values[switched_value_idx-3], switched_values[switched_value_idx-2]); // 13-14 
        }


       }

      //Final compare8
      

       switched_values[24] = _mm256_lddqu_si256((__m256i*)(imgSrcExt + component_offset + rd_offset));

      compare_exchange(switched_values[20], switched_values[24]);
      compare_exchange(switched_values[22], switched_values[24]);
      compare_exchange(switched_values[21], switched_values[24]);
      compare_exchange(switched_values[23], switched_values[24]);


       compare_exchange(switched_values[16], switched_values[20]); // 0-4, 8-12, 16-20, 24-28
       compare_exchange(switched_values[18], switched_values[22]); // 2-6, 10-14, 18-22, 26-30
       compare_exchange(switched_values[18], switched_values[20]); // 2-4, 10-12, 18-20, 26-28

       compare_exchange(switched_values[17], switched_values[21]); // 1-5, 9-13, 17-21, 25-29
       compare_exchange(switched_values[19], switched_values[23]); // 3-7, 11-15, 19-23, 27-31
       compare_exchange(switched_values[19], switched_values[21]); // 3-5, 11-13, 19-21, 27-29

       compare_exchange(switched_values[17], switched_values[18]); // 1-2, 9-10, 17-18, 25-26
       compare_exchange(switched_values[19], switched_values[20]); // 3-4, 11-12, 19-20, 27-28
       compare_exchange(switched_values[21], switched_values[22]); // 5-6, 13-14, 21-22, 29-30



       //Compare32

       compare_exchange(switched_values[0], switched_values[16]);
       compare_exchange(switched_values[8], switched_values[24]);
       compare_exchange(switched_values[8], switched_values[16]);

       compare_exchange(switched_values[4], switched_values[20]);
       //      compare_exchange(switched_values[12], switched_values[28]);
       compare_exchange(switched_values[12], switched_values[20]);
      
       compare_exchange(switched_values[4], switched_values[8]);
       compare_exchange(switched_values[12], switched_values[16]);
       compare_exchange(switched_values[20], switched_values[24]);
      
       compare_exchange(switched_values[2], switched_values[18]);
       //      compare_exchange(switched_values[10], switched_values[26]);
       compare_exchange(switched_values[8], switched_values[24]);
       compare_exchange(switched_values[10], switched_values[18]);
     
       compare_exchange(switched_values[6], switched_values[22]);       //      compare_exchange(switched_values[switched_value_idx-12], switched_values[switched_value_idx+4]);
      
       compare_exchange(switched_values[14], switched_values[22]);
       compare_exchange(switched_values[6], switched_values[10]);
       compare_exchange(switched_values[14], switched_values[18]);
       //      compare_exchange(switched_values[22], switched_values[26]);
      
       compare_exchange(switched_values[2], switched_values[4]);
       compare_exchange(switched_values[6], switched_values[8]);
       compare_exchange(switched_values[10], switched_values[12]);
       compare_exchange(switched_values[14], switched_values[16]);
       compare_exchange(switched_values[18], switched_values[20]);
       compare_exchange(switched_values[22], switched_values[24]);


       compare_exchange(switched_values[1], switched_values[17]);
//       compare_exchange(switched_values[9], switched_values[25]);
       compare_exchange(switched_values[9], switched_values[17]);
 
       compare_exchange(switched_values[5], switched_values[21]);
       //      compare_exchange(switched_values[13], switched_values[29]);
       compare_exchange(switched_values[13], switched_values[21]);

       compare_exchange(switched_values[5], switched_values[9]);
       compare_exchange(switched_values[13], switched_values[17]);
      // compare_exchange(switched_values[21], switched_values[25]);

       compare_exchange(switched_values[3], switched_values[19]);
       //      compare_exchange(switched_values[11], switched_values[27]);
       //compare_exchange(switched_values[9], switched_values[25]);
       compare_exchange(switched_values[11], switched_values[19]);

       compare_exchange(switched_values[7], switched_values[23]);
       //      compare_exchange(switched_values[15], switched_values[31]);
       compare_exchange(switched_values[15], switched_values[23]);
       compare_exchange(switched_values[7], switched_values[11]);
       compare_exchange(switched_values[15], switched_values[19]);
       //      compare_exchange(switched_values[23], switched_values[27]);

       compare_exchange(switched_values[3], switched_values[5]);
       compare_exchange(switched_values[7], switched_values[9]);
       compare_exchange(switched_values[11], switched_values[13]);
       compare_exchange(switched_values[15], switched_values[17]);
       compare_exchange(switched_values[19], switched_values[21]);
       //compare_exchange(switched_values[23], switched_values[25]);


       for(int i=0; i<24; i=i+2)
       {
        compare_exchange(switched_values[i+1], switched_values[i+2]);
       }
      int wr_offset;

      wr_offset = row*imgWidth*3 + col;
//      printf("Writing to imgDst[%d .. %d]\n", wr_offset, wr_offset + 31);
      __m256i results = switched_values[12];
      _mm256_storeu_si256((__m256i*)(imgDst + wr_offset), results);
     }
   }
}
















