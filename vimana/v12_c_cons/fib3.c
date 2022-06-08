// 40 --> This is the number for fib! Read by the program!

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long fib(long n)
{
  if (n < 2)
    return n;
  else
    return fib(n - 1) + fib(n - 2);
}

int read_int()
{
  char comment_token[8];
  int n;
  FILE* file = fopen("fib3.c", "r");
  fscanf(file, "%s %d", comment_token, &n);
  return n;
}

void fib_test()
{
  int n = 40; // read_int();
  long res = fib(n);
  printf("%li\n", res);
}

int main()
{
  fib_test();

  return 0;
}

/*
fib(37):

miki@mikis-MacBook-Air v12_c_cons % cc fib3.c   
miki@mikis-MacBook-Air v12_c_cons % time ./a.out
24157817
./a.out  0.16s user 0.00s system 68% cpu 0.238 total

miki@mikis-MacBook-Air v12_c_cons % cc fib3.c -Ofast
miki@mikis-MacBook-Air v12_c_cons % time ./a.out    
24157817
./a.out  0.07s user 0.00s system 49% cpu 0.147 total

fib(40):

miki@mikis-MacBook-Air v12_c_cons % cc fib3.c       
miki@mikis-MacBook-Air v12_c_cons % time ./a.out
102334155
./a.out  0.65s user 0.00s system 89% cpu 0.733 total

miki@mikis-MacBook-Air v12_c_cons % cc fib3.c -Ofast
miki@mikis-MacBook-Air v12_c_cons % time ./a.out    
102334155
./a.out  0.29s user 0.00s system 79% cpu 0.365 total

miki@mikis-MacBook-Air v12_c_cons % cc fib3.c -Ofast
miki@mikis-MacBook-Air v12_c_cons % time ./a.out    
102334155
./a.out  0.33s user 0.00s system 98% cpu 0.343 total

miki@mikis-MacBook-Air v12_c_cons % cc fib3.c -Ofast
miki@mikis-MacBook-Air v12_c_cons % time ./a.out
102334155
./a.out  0.34s user 0.00s system 97% cpu 0.349 total
miki@mikis-MacBook-Air v12_c_cons % time ./a.out
*/
