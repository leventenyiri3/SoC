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
  uint8_t d, merge_index;
  merge_index = 0;
  d = 0;


  uint8_t inner_array[32] = {}; 
  for(int i=0; i<32; i++)
      inner_array[i] = *(array + i);
  //ide nyilván ismétlés helyett logikusabb lenne ciklussal, de a párhuzamosításnál így talán jobban átlátom
  for(int base=0; base<32; base+=4)
  {

  compare(inner_array, base, base+1); // ez a kettő még mehet párhuzamosan akár
  compare(inner_array, base+2, base+3); //
  compare(inner_array, base, base+2);
  compare(inner_array, base+1, base+3);
  compare(inner_array, base+1, base+2);

  printf("\n");
  printf("a = %d", base);
  printf("\n");
  for(int i=0; i<32; i++)
    printf("%d ", inner_array[i]);

    if(base%8 == 0 && base != 0)
    {
      compare(inner_array, merge_index, merge_index+4);
      compare(inner_array, merge_index+2, merge_index+6);
      compare(inner_array, merge_index+2, merge_index+4);
      compare(inner_array, merge_index+1, merge_index+5);
      compare(inner_array, merge_index+3, merge_index+7);
      compare(inner_array, merge_index+3, merge_index+5);

      compare(inner_array, merge_index+1, merge_index+2);
      compare(inner_array, merge_index+3, merge_index+4);
      compare(inner_array, merge_index+5, merge_index+6);
    }

  

  }


// ez előtt lévők mehetnek full párhuzamosan, itt be kell várniuk
/* Nagyobb mergelés itt kezdődik*/

  compare(inner_array, 0+d, 8+d);
  compare(inner_array, 4+d, 12+d);
  compare(inner_array, 4+d, 8+d);
  compare(inner_array, 2+d, 10+d);
  compare(inner_array, 6+d, 14+d);
  compare(inner_array, 6+d, 10+d);
  compare(inner_array, 0+d, 2+d);
  compare(inner_array, 4+d, 6+d);
  compare(inner_array, 8+d, 10+d);
  compare(inner_array, 12+d, 14+d);

  d = 1;

  compare(inner_array, 0+d, 8+d);
  compare(inner_array, 4+d, 12+d);
  compare(inner_array, 4+d, 8+d);
  compare(inner_array, 2+d, 10+d);
  compare(inner_array, 6+d, 14+d);
  compare(inner_array, 6+d, 10+d);
  compare(inner_array, 0+d, 2+d);
  compare(inner_array, 4+d, 6+d);
  compare(inner_array, 8+d, 10+d);
  compare(inner_array, 12+d, 14+d);


  d = 16;
  
  compare(inner_array, 0+d, 8+d);
  compare(inner_array, 4+d, 12+d);
  compare(inner_array, 4+d, 8+d);
  compare(inner_array, 2+d, 10+d);
  compare(inner_array, 6+d, 14+d);
  compare(inner_array, 6+d, 10+d);
  compare(inner_array, 0+d, 2+d);
  compare(inner_array, 4+d, 6+d);
  compare(inner_array, 8+d, 10+d);
  compare(inner_array, 12+d, 14+d);
  
  d = 17;
  
  compare(inner_array, 0+d, 8+d);
  compare(inner_array, 4+d, 12+d);
  compare(inner_array, 4+d, 8+d);
  compare(inner_array, 2+d, 10+d);
  compare(inner_array, 6+d, 14+d);
  compare(inner_array, 6+d, 10+d);
  compare(inner_array, 0+d, 2+d);
  compare(inner_array, 4+d, 6+d);
  compare(inner_array, 8+d, 10+d);
  compare(inner_array, 12+d, 14+d);

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
