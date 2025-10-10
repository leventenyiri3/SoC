#include <stdio.h>
#include <stdbool.h>

void OddEvenMergeSort(int* array, int size)
{
  bool isSorted = false;

  while(isSorted == false)
  {
  
    int i = 0;
    isSorted = true;
    while(i+1< size)
    {
      if(*(array+i)>*(array+i+1))
      {
          int tmp = *(array+i+1);
          *(array+i+1) = *(array+i);
          *(array+i) = tmp;
          isSorted = false;
      }
      i = i+2;
    }
    
    i = 1;

    while(i+1< size)
    {
      if(*(array+i)>*(array+i+1))
      {
          int tmp = *(array+i+1);
          *(array+i+1) = *(array+i);
          *(array+i) = tmp;
          isSorted = false;
      }
      i = i+2;
    }
  } 
 return; 
}


int main()
{
  int array[] = {3,2,4,1,5,7,2,3,9,1};
  printf("Before sorting: \n");
  for(int i=0; i<10;i++)
    printf("%d ", array[i]);
  
  printf("\n");

  OddEvenMergeSort(array, 10);
  printf("After sorting: \n");
  for(int i=0; i<10;i++)
    printf("%d ", array[i]);

}

