#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
Tests:

Array with 128 bit struct:
- pointer based access
- copying items

Array with 64 bit struct:
- pointer based access
- copying items

Operations:
- Function pointers
- If statements
*/

//#define TEST_FUNPTR
// No random function selection:
// -Ofast
// ./a.out  3.86s user 0.20s system 95% cpu 4.252 total
// ./a.out  3.89s user 0.20s system 93% cpu 4.391 total
// ./a.out  3.90s user 0.20s system 92% cpu 4.451 total
// ./a.out  3.87s user 0.21s system 99% cpu 4.092 total
// No opt
// ./a.out  4.53s user 0.23s system 92% cpu 5.168 total
// With randpm function selection:
// No opt - rand function selection
// ./a.out  6.95s user 0.22s system 95% cpu 7.480 total
// Opt - random function selection
// ./a.out  5.26s user 0.21s system 94% cpu 5.815 total
// With 20000000 items 10 loops: 
// ./a.out  2.12s user 0.09s system 99% cpu 2.221 total

//#define TEST_FUNPTR_LOOPPTR
// ./a.out  3.86s user 0.20s system 90% cpu 4.466 total
// ./a.out  3.88s user 0.20s system 99% cpu 4.089 total

//#define TEST_FUNPTR_COPYITEM
// ./a.out  3.95s user 0.21s system 91% cpu 4.546 total
// ./a.out  3.97s user 0.21s system 99% cpu 4.193 total
// New test 210928:
// No opt: ./a.out  4.15s user 0.22s system 99% cpu 4.379 total
// Opt:    ./a.out  3.94s user 0.21s system 91% cpu 4.541 total

//#define TEST_FUNPTR_COPYITEM_FASTER
// New test 210928:
// No opt: ./a.out  4.15s user 0.22s system 93% cpu 4.676 total
// Opt:    ./a.out  3.94s user 0.22s system 91% cpu 4.552 total 

//#define TEST_PLAIN_WITHOUT_IF_ITEM64
// Without if:
// ./a.out  3.93s user 0.10s system 93% cpu 4.339 total

//#define TEST_PLAIN_WITHOUT_IF
// Without if:
// ./a.out  3.99s user 0.20s system 93% cpu 4.495 total
// ./a.out  4.04s user 0.21s system 93% cpu 4.533 total
// No opt
// ./a.out  3.97s user 0.18s system 92% cpu 4.462 total

//#define TEST_PLAIN_WITH_IF
// With 6 ifs
// ./a.out  4.07s user 0.20s system 85% cpu 4.979 total
// With 16 ifs
// ./a.out  5.08s user 0.20s system 94% cpu 5.605 total
// ./a.out  5.10s user 0.20s system 99% cpu 5.311 total
// ./a.out  5.44s user 0.21s system 94% cpu 5.975 total
// With opt
// ./a.out  5.61s user 0.20s system 94% cpu 6.159 total
// No opt
// ./a.out  10.14s user 0.21s system 97% cpu 10.661 total

#define TEST_FUNPTR_NOARRAYTRAVERSAL
// With -Ofast
// ./a.out  7.77s user 0.21s system 97% cpu 8.183 total
// It is much slower to not precompute function calls.
// With precomputed functions (test case TEST_FUNPTR):
// ./a.out  5.25s user 0.21s system 96% cpu 5.679 total
// With random function selection commented out:
// ./a.out  4.31s user 0.22s system 94% cpu 4.815 total

// Rand funs: 500 000 000 iterations --> 5.27s 
// Rand funs: 200 000 000 iterations --> 2.12s 
// Vimana:    20 fact 10 000 000 times = 200 000 000 --> 8.5s

#define NUM_ITEMS 50000000
#define NUM_LOOPS 10

typedef struct __Item128
{
  void* ptr;
  long  value;
}
Item128;

typedef struct __Item64
{
  long  value;
}
Item64;

typedef void (*FunPtr)(void*);
typedef Item128 (*FunItem128)(Item128);

void RandomUpdate_Item128(void* p)
{
  Item128* item = p;
  item->value += (rand() % 11) - 5;
}

void RandomUpdate_Item128_2(void* p)
{
  Item128* item = p;
  item->value += (rand() % 3) - 2;
}

Item128 RandomUpdate_Item128_CopyItem(Item128 item)
{
  item.value += (rand() % 11) - 5;
  return item;
}

void* CreateArray_Item128()
{
  Item128* items = malloc(sizeof(Item128) * NUM_ITEMS);

  for (long i = 0; i < NUM_ITEMS; ++i)
  {
#if defined(TEST_FUNPTR_COPYITEM) || defined(TEST_FUNPTR_COPYITEM_FASTER)
    items[i].ptr = RandomUpdate_Item128_CopyItem;
#else
    if ((rand() % 10) > 4)
      items[i].ptr = RandomUpdate_Item128;
    else
      items[i].ptr = RandomUpdate_Item128_2;
#endif
    items[i].value = (rand() % 11) - 5;
  }

  return items;
}

void* CreateArray_Item64()
{
  Item64* items = malloc(sizeof(Item64) * NUM_ITEMS);

  for (long i = 0; i < NUM_ITEMS; ++i)
  {
    items[i].value = (rand() % 11) - 5;
  }

  return items;
}

void Interpret_Item128_WithIf(Item128* items)
{
  printf("TEST PLAIN WITH IF\n");

  long tally = 0;

  long value;

  for (long j = 0; j < NUM_LOOPS; ++j)
  {
    for (long i = 0; i < NUM_ITEMS; ++i)
    {
      value = items[i].value;
      if (value < -80)
        items[i].value += (rand() % 80) - 5;
      else
      if (value < -70)
        items[i].value += (rand() % 70) - 5;
      else
      if (value < -60)
        items[i].value += (rand() % 60) - 5;
      else
      if (value < -50)
        items[i].value += (rand() % 50) - 5;
      else
      if (value < -40)
        items[i].value += (rand() % 40) - 5;
      else
      if (value < -30)
        items[i].value += (rand() % 30) - 5;
      else
      if (value < -20)
        items[i].value += (rand() % 20) - 3;
      else
      if (value < -10)
        items[i].value += (rand() % 15) - 4;
      else
      if (value > 80)
        items[i].value += (rand() % 10) - 100;
      else
      if (value > 70)
        items[i].value += (rand() % 10) - 80;
      else
      if (value > 60)
        items[i].value += (rand() % 10) - 70;
      else
      if (value > 50)
        items[i].value += (rand() % 10) - 60;
      else
      if (value > 40)
        items[i].value += (rand() % 10) - 50;
      else
      if (value > 30)
        items[i].value += (rand() % 10) - 25;
      else
      if (value > 20)
        items[i].value += (rand() % 10) - 15;
      else
      if (value > 10)
        items[i].value += (rand() % 10) - 5;
      else
        items[i].value += (rand() % 11) - 5;
      
      tally += items[i].value;
    }
    printf("LOOP COMPLETED: %ld\n", j);
  }
  printf("TALLY: %ld\n", tally);
}

void Interpret_Item128_WithoutIf(Item128* items)
{
  printf("TEST PLAIN WITHOUT IF\n");

  long tally = 0;

  for (long j = 0; j < NUM_LOOPS; ++j)
  {
    for (long i = 0; i < NUM_ITEMS; ++i)
    {
      items[i].value += (rand() % 11) - 5;
      tally += items[i].value;
    }
    printf("LOOP COMPLETED: %ld\n", j);
  }
  printf("TALLY: %ld\n", tally);
}

void Interpret_Item64_WithoutIf(Item64* items)
{
  printf("ITEM64 TEST PLAIN WITHOUT IF\n");

  long tally = 0;

  for (long j = 0; j < NUM_LOOPS; ++j)
  {
    for (long i = 0; i < NUM_ITEMS; ++i)
    {
      items[i].value += (rand() % 11) - 5;
      tally += items[i].value;
    }
    printf("LOOP COMPLETED: %ld\n", j);
  }
  printf("TALLY: %ld\n", tally);
}

void Interpret_Item128_CallFun(Item128* items)
{
  printf("TEST FUNPTR\n");

  long tally = 0;

  for (long j = 0; j < NUM_LOOPS; ++j)
  {
    for (long i = 0; i < NUM_ITEMS; ++i)
    {
      FunPtr fun = items[i].ptr;
      fun(&items[i]);
      tally += items[i].value;
    }
    printf("LOOP COMPLETED: %ld\n", j);
  }
  printf("TALLY: %ld\n", tally);
}

void Interpret_Item128_CallFun_LoopPtr(Item128* items)
{
  printf("TEST FUNPTR LOOPPTR\n");

  long tally = 0;

  for (long j = 0; j < NUM_LOOPS; ++j)
  {
    Item128* p = items;
    for (long i = 0; i < NUM_ITEMS; ++i)
    {
      FunPtr fun = p->ptr;
      fun(p);
      tally += p->value;
      ++p;
    }
    printf("LOOP COMPLETED: %ld\n", j);
  }
  printf("TALLY: %ld\n", tally);
}

void Interpret_Item128_CallFun_CopyItem(Item128* items)
{
  printf("TEST FUNPTR COPYITEM\n");

  long tally = 0;

  for (long j = 0; j < NUM_LOOPS; ++j)
  {
    for (long i = 0; i < NUM_ITEMS; ++i)
    {
      FunItem128 fun = items[i].ptr;
      items[i] = fun(items[i]);
      tally += items[i].value;
    }
    printf("LOOP COMPLETED: %ld\n", j);
  }
  printf("TALLY: %ld\n", tally);
}

void Interpret_Item128_CallFun_CopyItem_Faster(Item128* items)
{
  printf("TEST FUNPTR COPYITEM FASTER\n");

  long tally = 0;

  for (long j = 0; j < NUM_LOOPS; ++j)
  {
    for (long i = 0; i < NUM_ITEMS; ++i)
    {
      Item128* item = items + i;
      FunItem128 fun = item->ptr;
      *item = fun(*item);
      tally += item->value;
    }
    printf("LOOP COMPLETED: %ld\n", j);
  }
  printf("TALLY: %ld\n", tally);
}

void Interpret_Item128_NoArrayTraversal(Item128* items)
{
  printf("TEST FUNPTR NO ARRAY TRAVERSAL\n");

  long tally = 0;
  Item128 item;
  item.value = 0;

  for (long j = 0; j < NUM_LOOPS; ++j)
  {
    for (long i = 0; i < NUM_ITEMS; ++i)
    {
      //if ((rand() % 10) > 4)
        RandomUpdate_Item128(&item);
      //else
      //  RandomUpdate_Item128_2(&item);
      tally += item.value;
    }
    printf("LOOP COMPLETED: %ld\n", j);
  }
  printf("TALLY: %ld\n", tally);
}

int main()
{
  time_t t;
  srand((unsigned) time(&t));

#ifdef TEST_PLAIN_WITHOUT_IF_ITEM64
  void* items = CreateArray_Item64();
#else
  void* items = CreateArray_Item128();
#endif

  Item128* p = items;
  p = p + (rand() % 1000000);
  printf("RANDOM ARRAY ITEM: %ld\n", p->value);
  
#ifdef TEST_PLAIN_WITHOUT_IF
  Interpret_Item128_WithoutIf(items);
#endif

#ifdef TEST_PLAIN_WITHOUT_IF_ITEM64
  Interpret_Item64_WithoutIf(items);
#endif

#ifdef TEST_PLAIN_WITH_IF
  Interpret_Item128_WithIf(items);
#endif

#ifdef TEST_FUNPTR
  Interpret_Item128_CallFun(items);
#endif

#ifdef TEST_FUNPTR_LOOPPTR
  Interpret_Item128_CallFun_LoopPtr(items);
#endif

#ifdef TEST_FUNPTR_COPYITEM
  Interpret_Item128_CallFun_CopyItem(items);
#endif

#ifdef TEST_FUNPTR_COPYITEM_FASTER
  Interpret_Item128_CallFun_CopyItem_Faster(items);
#endif

#ifdef TEST_FUNPTR_NOARRAYTRAVERSAL
  Interpret_Item128_NoArrayTraversal(items);
#endif
}
