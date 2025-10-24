#include <stdio.h>
#include "stdbool.h"
#include "stdint.h"

void compare(uint8_t* array, int i, int j)
{
  if (array[i] > array[j])
  {
    uint8_t tmp = array[i];
    array[i] = array[j];
    array[j] = tmp;
  }
}

void batcher_non_recursive(uint8_t* array)
{
  uint8_t inner_array[32] = {}; 
  for(int i = 0; i < 32; i++)
      inner_array[i] = array[i];
  
  // Sort 8 groups of 4 elements each
  for(int base = 0; base < 32; base += 4)
  {
    compare(inner_array, base, base+1);
    compare(inner_array, base+2, base+3);
    compare(inner_array, base, base+2);
    compare(inner_array, base+1, base+3);
    compare(inner_array, base+1, base+2);
  }
  
  // Merge pairs of 4-element groups into 8-element groups
  for(int base = 0; base < 32; base += 8)
  {
    compare(inner_array, base+0, base+4);
    compare(inner_array, base+1, base+5);
    compare(inner_array, base+2, base+6);
    compare(inner_array, base+3, base+7);
    
    compare(inner_array, base+0, base+2);
    compare(inner_array, base+1, base+3);
    compare(inner_array, base+4, base+6);
    compare(inner_array, base+5, base+7);
    
    compare(inner_array, base+1, base+2);
    compare(inner_array, base+3, base+4);
    compare(inner_array, base+5, base+6);
  }
  
  // Merge pairs of 8-element groups into 16-element groups
  for(int base = 0; base < 32; base += 16)
  {
    compare(inner_array, base+0, base+8);
    compare(inner_array, base+1, base+9);
    compare(inner_array, base+2, base+10);
    compare(inner_array, base+3, base+11);
    compare(inner_array, base+4, base+12);
    compare(inner_array, base+5, base+13);
    compare(inner_array, base+6, base+14);
    compare(inner_array, base+7, base+15);
    
    compare(inner_array, base+0, base+4);
    compare(inner_array, base+1, base+5);
    compare(inner_array, base+2, base+6);
    compare(inner_array, base+3, base+7);
    compare(inner_array, base+8, base+12);
    compare(inner_array, base+9, base+13);
    compare(inner_array, base+10, base+14);
    compare(inner_array, base+11, base+15);
    
    compare(inner_array, base+0, base+2);
    compare(inner_array, base+1, base+3);
    compare(inner_array, base+4, base+6);
    compare(inner_array, base+5, base+7);
    compare(inner_array, base+8, base+10);
    compare(inner_array, base+9, base+11);
    compare(inner_array, base+12, base+14);
    compare(inner_array, base+13, base+15);
    
    compare(inner_array, base+1, base+2);
    compare(inner_array, base+3, base+4);
    compare(inner_array, base+5, base+6);
    compare(inner_array, base+7, base+8);
    compare(inner_array, base+9, base+10);
    compare(inner_array, base+11, base+12);
    compare(inner_array, base+13, base+14);
  }
  
  // Final merge: combine two 16-element groups into one 32-element sorted array
  compare(inner_array, 0, 16);
  compare(inner_array, 1, 17);
  compare(inner_array, 2, 18);
  compare(inner_array, 3, 19);
  compare(inner_array, 4, 20);
  compare(inner_array, 5, 21);
  compare(inner_array, 6, 22);
  compare(inner_array, 7, 23);
  compare(inner_array, 8, 24);
  compare(inner_array, 9, 25);
  compare(inner_array, 10, 26);
  compare(inner_array, 11, 27);
  compare(inner_array, 12, 28);
  compare(inner_array, 13, 29);
  compare(inner_array, 14, 30);
  compare(inner_array, 15, 31);
  
  compare(inner_array, 0, 8);
  compare(inner_array, 1, 9);
  compare(inner_array, 2, 10);
  compare(inner_array, 3, 11);
  compare(inner_array, 4, 12);
  compare(inner_array, 5, 13);
  compare(inner_array, 6, 14);
  compare(inner_array, 7, 15);
  compare(inner_array, 16, 24);
  compare(inner_array, 17, 25);
  compare(inner_array, 18, 26);
  compare(inner_array, 19, 27);
  compare(inner_array, 20, 28);
  compare(inner_array, 21, 29);
  compare(inner_array, 22, 30);
  compare(inner_array, 23, 31);
  
  compare(inner_array, 0, 4);
  compare(inner_array, 1, 5);
  compare(inner_array, 2, 6);
  compare(inner_array, 3, 7);
  compare(inner_array, 8, 12);
  compare(inner_array, 9, 13);
  compare(inner_array, 10, 14);
  compare(inner_array, 11, 15);
  compare(inner_array, 16, 20);
  compare(inner_array, 17, 21);
  compare(inner_array, 18, 22);
  compare(inner_array, 19, 23);
  compare(inner_array, 24, 28);
  compare(inner_array, 25, 29);
  compare(inner_array, 26, 30);
  compare(inner_array, 27, 31);
  
  compare(inner_array, 0, 2);
  compare(inner_array, 1, 3);
  compare(inner_array, 4, 6);
  compare(inner_array, 5, 7);
  compare(inner_array, 8, 10);
  compare(inner_array, 9, 11);
  compare(inner_array, 12, 14);
  compare(inner_array, 13, 15);
  compare(inner_array, 16, 18);
  compare(inner_array, 17, 19);
  compare(inner_array, 20, 22);
  compare(inner_array, 21, 23);
  compare(inner_array, 24, 26);
  compare(inner_array, 25, 27);
  compare(inner_array, 28, 30);
  compare(inner_array, 29, 31);
  
  compare(inner_array, 1, 2);
  compare(inner_array, 3, 4);
  compare(inner_array, 5, 6);
  compare(inner_array, 7, 8);
  compare(inner_array, 9, 10);
  compare(inner_array, 11, 12);
  compare(inner_array, 13, 14);
  compare(inner_array, 15, 16);
  compare(inner_array, 17, 18);
  compare(inner_array, 19, 20);
  compare(inner_array, 21, 22);
  compare(inner_array, 23, 24);
  compare(inner_array, 25, 26);
  compare(inner_array, 27, 28);
  compare(inner_array, 29, 30);
  
  printf("\nAfter sorting:\n");
  for(int i = 0; i < 32; i++)
    printf("%d ", inner_array[i]);
  printf("\n");
  
  // Copy back to original array
  for(int i = 0; i < 32; i++)
    array[i] = inner_array[i];
}

int main()
{
  uint8_t array[] = {6,3,8,1,0,4,7,2,9,5,3,6,0,8,1,7,4,2,5,9,6,3,0,7,1,4,8,2,5,9,6,0};

  printf("Before sorting:\n");
  for(int i = 0; i < 32; i++)
    printf("%d ", array[i]);
  printf("\n");
  
  batcher_non_recursive(array);
  
  return 0;
}
