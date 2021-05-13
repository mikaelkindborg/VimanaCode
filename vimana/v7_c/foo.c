
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct MyItem
{
  unsigned long type;
  union
  {
    // Fields used by data lists and code.
    int     symbol; // Index in symbol table or local environment table
    double  decNum;
    long    intNum;
    void*   list;
    char*   string; // TODO: Make custom string object
  }
  value;
}
Item;

Item add(Item item1, Item item2)
{
  Item res;
  res.value.intNum = item1.value.intNum + item2.value.intNum;
  return res;
}

void addp(Item* item1, Item* item2)
{
  item1->value.intNum = item1->value.intNum + item2->value.intNum;
}

Item add1(Item item1)
{
  Item res;
  res.value.intNum = item1.value.intNum + 2;
  return res;
}

// 5 times faster than add1()
void add1p(Item* item1)
{
  item1->value.intNum += 2; // 1.50s
  //item1->value.intNum = item1->value.intNum + 2; // 1.29s !!!
}

int add2(int a, int b)
{
  return a + b;
}

void addItems1()
{
  Item item1;
  Item item2;
  item1.value.intNum = 1;
  item2.value.intNum = 2;
  int i = 0;
  while (i < 1000000000)
  {
    item1 = add(item1, item2); // 7.93s
    //item1 = add1(item1); // 7.58s
    //add1p(&item1); // 1.49s
    //item1.value.intNum = add2(item1.value.intNum, 2); // 1.61s
    i++;
  }
  printf("Result: %li\n", item1.value.intNum);
}

void addItems2()
{
  Item item1;
  Item item2;
  item1.value.intNum = 1;
  item2.value.intNum = 2;
  int i = 0;
  while (i < 1000000000)
  {
    item1.value.intNum = item1.value.intNum + item2.value.intNum;
    i++;
  }
  printf("Result: %li\n", item1.value.intNum);
}

void addItems3()
{
  Item item1;
  Item item2;
  item1.value.intNum = 1;
  item2.value.intNum = 2;
  int i = 0;
  while (i < 1000000000)
  {
    addp(&item1, &item2);
    i++;
  }
  printf("Result: %li\n", item1.value.intNum);
}

void addItems4A()
{
  Item item1;
  Item item2;
  item1.value.intNum = 1;
  item2.value.intNum = 2;
  Item* item1p = & item1;
  Item* item2p = & item2;
  int i = 0;
  while (i < 1000000000)
  {
    item1p->value.intNum = item1p->value.intNum + item2p->value.intNum;
    i++;
  }
  printf("Result: %li\n", item1.value.intNum);
}

void addItems4B()
{
  Item item1;
  Item item2;
  item1.value.intNum = 1;
  item2.value.intNum = 2;
  Item* item1p = & item1;
  Item* item2p = & item2;
  int i = 0;
  while (i < 1000000000)
  {
    (*item1p).value.intNum = (*item1p).value.intNum + (*item2p).value.intNum;
    i++;
  }
  printf("Result: %li\n", item1.value.intNum);
}


void addItems5()
{
  Item item[2];
  item[0].value.intNum = 1;
  item[1].value.intNum = 2;
  int i = 0;
  while (i < 1000000000)
  {
    item[0].value.intNum = item[0].value.intNum + item[1].value.intNum;
    i++;
  }
  printf("Result: %li\n", item[0].value.intNum);
}

void addItems6()
{
  Item item1;
  Item item2;
  Item temp;
  Item res;
  item1.value.intNum = 1;
  item2.value.intNum = 2;
  int i = 0;
  while (i < 1000000000)
  {
    // 0.99s
    //res.value.intNum = item1.value.intNum + item2.value.intNum;
    //item1 = res;

    // 1.32s
    temp = item1;
    res.value.intNum = temp.value.intNum + item2.value.intNum;
    item1 = res;
    
    // 0.72s
    //item1.value.intNum = item1.value.intNum + item2.value.intNum;
    
    i++;
  }
  printf("Result: %li\n", item1.value.intNum);
}

typedef void (*PrimFun)(void*);

int main()
{
  printf("Test of structs\n");
  
  int size = sizeof(Item);
  printf("Size of Item: %i\n", size);

  size = sizeof(unsigned char);
  printf("Size of uchar: %i\n", size);

  size = sizeof(int);
  printf("Size of int: %i\n", size);

  size = sizeof(long);
  printf("Size of long: %i\n", size);

  size = sizeof(double);
  printf("Size of double: %i\n", size);

  size = sizeof(void*);
  printf("Size of void*: %i\n", size);

  size = sizeof(PrimFun);
  printf("Size of PrimFun: %i\n", size);
  
  //addItems1(); // 7.93s (function call with structs)
  //addItems2(); // 0.77s (structs)
  //addItems3(); // 1.75s (function call with pointers)
  //addItems4A(); // 1.27s (pointers)
  addItems4B(); // 1.28s (pointers alternative syntax)
  //addItems5(); // 0.78s (array indexes)
  //addItems6(); // 1.08s (1 copy of Item), 1.30s (2 copies of Item)

  return 0;
}
