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

  compare(array, a, a+1); // ez a kettő még mehet párhuzamosan akár
  compare(array, c, c+1); //
  compare(array, a, a+2);
  compare(array, b, b+2);
  compare(array, b, b+1);

  a = 4;

  compare(array, a, a+1);
  compare(array, c, c+1);
  compare(array, a, a+2);
  compare(array, b, b+2);
  compare(array, b, b+1);


/* Ez a 8-al megnövelt mehet párhuzamosan az előzővel */
  a = 8;

  compare(array, a, a+1);
  compare(array, c, c+1);
  compare(array, a, a+2);
  compare(array, b, b+2);
  compare(array, b, b+1);

  a = 12;

  compare(array, a, a+1);
  compare(array, c, c+1);
  compare(array, a, a+2);
  compare(array, b, b+2);
  compare(array, b, b+1);
    

/* Nagyobb mergelés itt kezdődik*/

  compare(array, 0, 8);
  compare(array, 4, 12);
  compare(array, 4, 8);
  compare(array, 2, 10);
  compare(array, 6, 14);
  compare(array, 6, 10);
  compare(array, 0, 2);
  compare(array, 4, 6);
  compare(array, 8, 10);
  compare(array, 12, 14);


}
