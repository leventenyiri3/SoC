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
  uint8_t a,b,c,d;
  a = 0;
  b = a+1;
  c = a+2;
  d = 0;


  uint8_t inner_array[32] = {}; 
  for(int i=0; i<32; i++)
      inner_array[i] = *(array + i);
  //ide nyilván ismétlés helyett logikusabb lenne ciklussal, de a párhuzamosításnál így talán jobban átlátom
  
  compare(inner_array, a, a+1); // ez a kettő még mehet párhuzamosan akár
  compare(inner_array, a+2, a+3); //
  compare(inner_array, a, a+2);
  compare(inner_array, a+1, a+3);
  compare(inner_array, a+1, a+2);

  printf("\n");
  printf("a = %d", a);
  printf("\n");
  for(int i=0; i<32; i++)
    printf("%d ", inner_array[i]);
  a = 4;

  compare(inner_array, a, a+1); 
  compare(inner_array, a+2, a+3); 
  compare(inner_array, a, a+2);
  compare(inner_array, a+1, a+3);
  compare(inner_array, a+1, a+2);

  printf("\n");
  printf("a = %d", a);
  printf("\n");
  for(int i=0; i<32; i++)
    printf("%d ", inner_array[i]);

/* Ez a 8-al megnövelt mehet párhuzamosan az előzővel */
  a = 8;

  compare(inner_array, a, a+1); 
  compare(inner_array, a+2, a+3); 
  compare(inner_array, a, a+2);
  compare(inner_array, a+1, a+3);
  compare(inner_array, a+1, a+2);

  printf("\n");
  printf("a = %d", a);
  printf("\n");
  for(int i=0; i<32; i++)
    printf("%d ", inner_array[i]);
  a = 12;

  compare(inner_array, a, a+1);
  compare(inner_array, c, c+1);
  compare(inner_array, a, a+2);
  compare(inner_array, b, b+2);
  compare(inner_array, b, b+1);

  printf("\n");
  printf("a = %d", a);
  printf("\n");
  for(int i=0; i<32; i++)
    printf("%d ", inner_array[i]);
//másik fél

  a = 16;

  compare(inner_array, a, a+1);
  compare(inner_array, c, c+1);
  compare(inner_array, a, a+2);
  compare(inner_array, b, b+2);
  compare(inner_array, b, b+1);

  printf("\n");
  printf("a = %d", a);
  printf("\n");
  for(int i=0; i<32; i++)
    printf("%d ", inner_array[i]);
  a = 20;

  compare(inner_array, a, a+1);
  compare(inner_array, c, c+1);
  compare(inner_array, a, a+2);
  compare(inner_array, b, b+2);
  compare(inner_array, b, b+1);
  
  printf("\n");
  printf("a = %d", a);
  printf("\n");
  for(int i=0; i<32; i++)
    printf("%d ", inner_array[i]);
  a = 24;

  compare(inner_array, a, a+1);
  compare(inner_array, c, c+1);
  compare(inner_array, a, a+2);
  compare(inner_array, b, b+2);
  compare(inner_array, b, b+1);

  printf("\n");
  printf("a = %d", a);
  printf("\n");
  for(int i=0; i<32; i++)
    printf("%d ", inner_array[i]);
  a = 28;

  compare(inner_array, a, a+1);
  compare(inner_array, c, c+1);
  compare(inner_array, a, a+2);
  compare(inner_array, b, b+2);
  compare(inner_array, b, b+1);
  
  printf("\n");
  printf("a = %d", a);
  printf("\n");
  for(int i=0; i<32; i++)
    printf("%d ", inner_array[i]);
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
