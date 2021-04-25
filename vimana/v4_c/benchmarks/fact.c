#include <stdio.h>

long fact(long n)
{
  if (n == 0)
    return 1;
  else
    return fact(n - 1) * n;
}

int main()
{
  printf("Hello Factorial\n");

  //0.84s user 0.00s system 99% cpu 0.849 total
  
  long res;
  for (long i = 0; i < 10000000; ++i) 
  {
    res = fact(20);
    //printf("Fact: %li\n", res);
    res++;
  }
  printf("Fact: %li\n", res);

  return 0;
}
