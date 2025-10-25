#include <stdio.h>
#include "stdbool.h"
#include "stdint.h"

void compare(uint8_t* array, int i, int j)
{
  if (array[i]>array[j])
  {
    uint8_t tmp = array[i];
    array[i] = array[j];
    array[j] = tmp;
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

  uint8_t inner_array[32] = {}; 
  for(int i=0; i<32; i++)
      inner_array[i] = *(array + i);

  #pragma unroll
  for(int base=0; base<32; base += 4)
  {
    if(base != 0 && base%8 == 0)
    {
      compare(inner_array, merge_8, merge_8+4);
      compare(inner_array, merge_8+2, merge_8+6);
      compare(inner_array, merge_8+2, merge_8+4);
      compare(inner_array, merge_8+1, merge_8+5);
      compare(inner_array, merge_8+3, merge_8+7);
      compare(inner_array, merge_8+3, merge_8+5);
      compare(inner_array, merge_8+1, merge_8+2);
      compare(inner_array, merge_8+3, merge_8+4);
      compare(inner_array, merge_8+5, merge_8+6);

      merge_8 += 8;
    }

    if(merge_8 != 0 && base%16 == 0)
    {
      compare(inner_array, merge_16, merge_16+8);
      compare(inner_array, merge_16+4, merge_16+12);
      compare(inner_array, merge_16+4, merge_16+8);
      compare(inner_array, merge_16+2, merge_16+10);
      compare(inner_array, merge_16+6, merge_16+14);
      compare(inner_array, merge_16+6, merge_16+10);
      compare(inner_array, merge_16+2, merge_16+4);
      compare(inner_array, merge_16+6, merge_16+8);
      compare(inner_array, merge_16+10, merge_16+12);

      merge_16 += 1;

      compare(inner_array, merge_16, merge_16+8);
      compare(inner_array, merge_16+4, merge_16+12);
      compare(inner_array, merge_16+4, merge_16+8);
      compare(inner_array, merge_16+2, merge_16+10);
      compare(inner_array, merge_16+6, merge_16+14);
      compare(inner_array, merge_16+6, merge_16+10);
      compare(inner_array, merge_16+2, merge_16+4);
      compare(inner_array, merge_16+6, merge_16+8);
      compare(inner_array, merge_16+10, merge_16+12);

      merge_16 = 16;

      compare(inner_array, merge_16_for+1, merge_16_for+2);
      compare(inner_array, merge_16_for+3, merge_16_for+4);
      compare(inner_array, merge_16_for+5, merge_16_for+6);
      compare(inner_array, merge_16_for+7, merge_16_for+8);
      compare(inner_array, merge_16_for+9, merge_16_for+10);
      compare(inner_array, merge_16_for+11, merge_16_for+12);
      compare(inner_array, merge_16_for+13, merge_16_for+14); 

      merge_16_for += 16;
    }

    compare(inner_array, base, base+1);
    compare(inner_array, base+2, base+3);
    compare(inner_array, base, base+2);
    compare(inner_array, base+1, base+3);
    compare(inner_array, base+1, base+2);

    

  }

  compare(inner_array, merge_8, merge_8+4);
  compare(inner_array, merge_8+2, merge_8+6);
  compare(inner_array, merge_8+2, merge_8+4);
  compare(inner_array, merge_8+1, merge_8+5);
  compare(inner_array, merge_8+3, merge_8+7);
  compare(inner_array, merge_8+3, merge_8+5);
  compare(inner_array, merge_8+1, merge_8+2);
  compare(inner_array, merge_8+3, merge_8+4);
  compare(inner_array, merge_8+5, merge_8+6);


  compare(inner_array, merge_16, merge_16+8);
  compare(inner_array, merge_16+4, merge_16+12);
  compare(inner_array, merge_16+4, merge_16+8);
  compare(inner_array, merge_16+2, merge_16+10);
  compare(inner_array, merge_16+6, merge_16+14);
  compare(inner_array, merge_16+6, merge_16+10);
  compare(inner_array, merge_16+2, merge_16+4);
  compare(inner_array, merge_16+6, merge_16+8);
  compare(inner_array, merge_16+10, merge_16+12);

  merge_16 += 1;

  compare(inner_array, merge_16, merge_16+8);
  compare(inner_array, merge_16+4, merge_16+12);
  compare(inner_array, merge_16+4, merge_16+8);
  compare(inner_array, merge_16+2, merge_16+10);
  compare(inner_array, merge_16+6, merge_16+14);
  compare(inner_array, merge_16+6, merge_16+10);
  compare(inner_array, merge_16+2, merge_16+4);
  compare(inner_array, merge_16+6, merge_16+8);
  compare(inner_array, merge_16+10, merge_16+12);

  merge_16 = 16;

  compare(inner_array, merge_16_for+1, merge_16_for+2);
  compare(inner_array, merge_16_for+3, merge_16_for+4);
  compare(inner_array, merge_16_for+5, merge_16_for+6);
  compare(inner_array, merge_16_for+7, merge_16_for+8);
  compare(inner_array, merge_16_for+9, merge_16_for+10);
  compare(inner_array, merge_16_for+11, merge_16_for+12);
  compare(inner_array, merge_16_for+13, merge_16_for+14); 

  #pragma unroll
  for(int merge_32=0; merge_32<2; merge_32++)
    {
      compare(inner_array, merge_32+0, merge_32+16);
      compare(inner_array, merge_32+8, merge_32+24);
      compare(inner_array, merge_32+8, merge_32+16);
      compare(inner_array, merge_32+8, merge_32+24);
      compare(inner_array, merge_32+4, merge_32+20);
      compare(inner_array, merge_32+12, merge_32+28);
      compare(inner_array, merge_32+12, merge_32+20);
      compare(inner_array, merge_32+4, merge_32+8);
      compare(inner_array, merge_32+12, merge_32+16);
      compare(inner_array, merge_32+20, merge_32+24);
      compare(inner_array, merge_32+2, merge_32+18);
      compare(inner_array, merge_32+10, merge_32+26);
      compare(inner_array, merge_32+8, merge_32+24);
      compare(inner_array, merge_32+10, merge_32+18);
      compare(inner_array, merge_32+6, merge_32+22);
      compare(inner_array, merge_32+14, merge_32+30);
      compare(inner_array, merge_32+14, merge_32+22);
      compare(inner_array, merge_32+6, merge_32+10);
      compare(inner_array, merge_32+14, merge_32+18);
      compare(inner_array, merge_32+22, merge_32+26);

      compare(inner_array, merge_32+2, merge_32+4);
      compare(inner_array, merge_32+6, merge_32+8);
      compare(inner_array, merge_32+10, merge_32+12);
      compare(inner_array, merge_32+14, merge_32+16);
      compare(inner_array, merge_32+18, merge_32+20);
      compare(inner_array, merge_32+22, merge_32+24);
      compare(inner_array, merge_32+26, merge_32+28);
    }

  #pragma unroll
  for(int i=1; i<30; i+=2)
  {
    compare(inner_array, i, i+1);
  }

  printf("\n");
  printf("After sorting:\n");
  for(int i=0; i<32; i++)
    printf("%d ", inner_array[i]);

}


int main()
{
  uint8_t array[] = {6,3,8,1,0,4,7,2,9,5,3,6,0,8,1,7,4,2,5,9,6,3,0,7,1,4,8,2,5,9,6,0};

  printf("Before sorting:\n");
  for(int i=0; i<32; i++)
    printf("%d ", array[i]);
  
  batcher_non_recursive(array);



}
