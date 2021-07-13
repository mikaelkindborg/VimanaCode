// 20
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

void fact_test_rand()
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

void fact_test(int n)
{
  long res = 0;
  for (long i = 0; i < 10000000; ++i) 
  {
    res = res + fact(n);
  }
  printf("Fact: %li\n", res);
}

int read_int()
{
  char comment[1024];
  int n;
  FILE* file = fopen("fact.c", "r");
  fscanf(file, "%s %d", comment, &n);
  return n;
}

int main()
{
  //printf("Hello Factorial\n");

  // Test 210713
  fact_test_rand();
  //miki@mikis-Air benchmarks % cc fact.c 
  //miki@mikis-Air benchmarks % time ./a.out
  //./a.out  0.90s user 0.00s system 72% cpu 1.248 total
  //miki@mikis-Air benchmarks % cc fact.c -O3
  //miki@mikis-Air benchmarks % time ./a.out 
  //./a.out  0.18s user 0.00s system 38% cpu 0.486 total
  
  // Test 210713
  //int n = read_int(); fact_test(n);
  //miki@mikis-Air benchmarks % cc fact.c -O3
  //miki@mikis-Air benchmarks % time ./a.out 
  //./a.out  0.08s user 0.00s system 19% cpu 0.423 total
  //miki@mikis-Air benchmarks % cc fact.c
  //miki@mikis-Air benchmarks % time ./a.out 
  //./a.out  0.78s user 0.00s system 69% cpu 1.127 total

  return 0;
}
