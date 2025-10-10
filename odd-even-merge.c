#include <stdio.h>
#include "stdbool.h"

void compare(int* array, int i, int j)
{
  if (array[i]>array[j])
  {
    int tmp = array[i];
    array[i] = array[j];
    array[j] = tmp;
  }

}

void merge(int* array, int start, int length, int r)
{
  int m=r*2;
  if(m<length)
  {
    merge(array, start, length, m);
    merge(array, start+r, length, m);
    for (int i=start+r; i+r<start+length; i+=m)
      compare(array, i, i+r);
  }
  else
    compare(array, start, start+r);
}

void sort(int* array, int start, int length)
{

  if(length>1)
  {
    int m = length/2;
    sort(array, start, m);
    sort(array, start+m, m); //ennek a sortnak annyi a lényege, hogy beállítsuk start és length értékét amit átadunk a merge-nek
    merge(array, start, length, 1);
  }

}


int main()
{
  int array[] = { 6, 3, 8, 1, 0, 4, 7, 2, 9, 5, 3, 6, 0, 8, 1, 7, 4, 2, 5, 9, 6, 3, 0, 7, 1, 4, 8, 2, 5, 9, 6, 0};

  printf("Before sorting:\n");
  for(int i=0; i<32; i++)
    printf("%d ", array[i]);
  
  sort(array, 0, 32);

  printf("\n");
  printf("After sorting:\n");
  for(int i=0; i<32; i++)
    printf("%d ", array[i]);

}
