#include <stdio.h>
#include <stdlib.h>

int main()
{
  int row = 34;
  int col = 34;
  int rand_int;
  int i,j;
  double rand_double;

  srand(123);
  for (i = 0; i < row; i++)
    {
      for (j = 0; j < col; j++)
	{
	  rand_int = rand();
	  rand_double = rand_int / 31415926.53589795;
	  printf("%.15f,",rand_double);
	}
      printf("\r\n");
    }

  return 0;
}

