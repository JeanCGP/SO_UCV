#include <stdio.h>
#include "arit.h"

int main(void)
{
  int a, b;
  printf("Operando 1: ");
  scanf("%d",&a);
  printf("Operando 2: ");
  scanf("%d",&b);
  printf("%d+%d=%d\n",a,b,suma(a,b));
  printf("%d*%d=%d\n",a,b,mult(a,b));
  return 0;
}
