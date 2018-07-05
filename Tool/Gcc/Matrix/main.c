#include "types.h"
#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>

#define   X1_ROW  10
#define   X1_COL  10
#define   X2_ROW  10
#define   X2_COL  10
int main()
{
  Matrix hX1;
  Matrix hX2;
  Matrix hX3;
  int16u i, j;
  int32s rand_int;
  float64 rand_double;

  float64 X1[X1_ROW][X1_COL];
  float64 X2[X2_ROW][X2_COL];
  float64 X3[X1_ROW][X2_COL];

  MatrixInit(&hX1, X1_ROW, X1_COL, X1);
  MatrixInit(&hX2, X2_ROW, X2_COL, X2);
  MatrixInit(&hX3, X1_ROW, X2_COL, X3);

  srand(123);
  for(i=0; i<X1_ROW; i++)
    {
      for(j=0; j<X1_COL; j++)
	{
	  rand_int = rand();
	  rand_double = rand_int / 31415926.53589795;
	  X1[i][j] = rand_double;
	  rand_int = rand();
	  rand_double = rand_int / 31415926.53589795;
	  X2[j][i] = rand_double;
	}
    }
  //hw_MatrixMul(&hX1, &hX2, &hX3);
  sw_MatrixInverse(&hX1, &hX3);
#if 1
  for (i = 0; i < X1_ROW; i++)
    {
      for (j = 0; j < X2_COL; j++)
	{
	  printf("   %f",X3[i][j]);
	}
      printf("\r\n");
    }
#endif

#if 0
  for (i = 0; i < X1_ROW; i++)
    {
      for (j = 0; j < X1_COL; j++)
	{
	  printf("   %f",X1[i][j]);
	}
      printf("\r\n");
    }
#endif

#if 0
  for (i = 0; i < X2_ROW; i++)
    {
      for (j = 0; j < X2_COL; j++)
	{
	  printf("   %f",X2[i][j]);
	}
      printf("\r\n");
    }
#endif
  return 0;
}

