#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long fact(long n)
{
  if (n == 0)
    return 1;
  else
    return fact(n - 1) * n;
}

void fact1()
{
  time_t t;
  long res = 0;
  srand((unsigned) time(&t));
  for (long i = 0; i < 10000000; ++i) 
  {
    res = res + fact(19 + (rand() % 2));
    //printf("Fact: %li\n", res);
  }
  printf("Fact: %li\n", res);
}

int main()
{
  printf("Hello Factorial\n");

  //0.84s user 0.00s system 99% cpu 0.849 total
  
  fact1();

  return 0;
}
