
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

/*

(process)
  (nextmsg eval) def


symbol dictionary is an array of immutable strings (malloc/memblock allocated)

pointers can be local to memory blocks 16 bit

symbol table is an array with values - a variable is an index into the array,
these are like environments/contexts

call stack is a memory block with stackframes (VStackMem)

stackframe
  first item
  current item (instr pointer)
  num slots
  register slots, dynamically allocated

data stack is a memory block (VStackMem)

list space consists of items in one memory block (VListMem)

strings are malloc allocated buffers (VBufMem)

"Compiling"
when parser/compiler finds "fun" or "def", the list is "compiled",
this means that variables are replaced with set and get instructions
to stackframe registers/localvars
(...
 "(x)set" - means add x to local compile table and insert set instr
 "(fun code)funify" - means compile list
 "x" means insert get instr
) fun


(x fact) (
  x iszero (1) (x x sub1 fact *) ifelse) def
rewrite to:
((x)set x iszero (1) (x x sub1 fact *) ifelse) funify (fact)setglobal
rewrite to:
(reg1_set reg1_get iszero (1) (reg1_get reg1_get sub1 fact *) ifelse)
where "reg1_set" and "reg1_get" are primitives

call also funifies, call is like apply



(fact) (r1!
  r1 iszero (1) (r1 sub1 fact r1 *) ifelse) def

(fact) (@1!
  @1 iszero (1) (@1 sub1 fact @1 *) ifelse) def



(fact) (
  1 param
  p1 iszero (1) (p1 sub1 fact p1 *) ifelse) def

(fact) ((x z)local (x)set
  x iszero (1) (x sub1 fact x *) ifelse) def

Some kind of jitting

fun orig list, compiled list

(a b) set (two local var entries created and set)

(x fact)
  (x iszero (1) (x sub1 fact x *) ifelse) def

((x)
  x iszero (1) (x sub1 fact x *) ifelse) fun (fact)setglobal

(x) (x iszero (1) (x sub1 fact x *) ifelse) fun (fact) setglobal


((a b | f)
  (+)@ a b ((f a b) (a b f)) fun apply)

((a b | f)
  ((f a b) (a b f)) fun (f)set
  (+)@ a b f apply) fun (test)gset

((a b | f) test
  ((f a b) (a b f)) fun (f)set
  (+)@ a b f apply) fun (test)gset


((a b) set
  ((f a b) (a b f)) fun (f)set
  (+)@ a b f apply) fun (test)gset) call


(test) ((a b):
  ((f a b): a b f) fun (f)set
  (+)@ a b f apply
) def

(foo) (a b) 
  (a b +) def

(a b foo)
  (a b +) def

(foo) (
  ((add)
   1 2 add print
   40 2 add print) fun
  ((a b) a b +) fun
    apply
) def





 (a b foo)
  (a b +) def

(a b)set

(a b test) (f)


(fact) ((x) =>
  x iszero (1) (x sub1 fact x *) ifelse) def

(fact) ((x)
  x iszero (1) (x sub1 fact x *) ifelse) def

(fact) (
  dup iszero (drop 1) (dup sub1 fact *) ifelse) def

(x fact) (
  x iszero (1) (x x sub1 fact *) ifelse) def

*/


/*
TODO:
Parser
Symbol table
Callstack


(a) first nil cons -> (a)
(a b c) 

(length) ((list):
  list empty? (0) (list rest length 1 +) ifelse
) def

(list length -> number) doc
(length) (
  dup empty? (drop 0) (rest length 1 +) ifelse
) def

(copy) (
  dup empty? (drop nil) (dup first swap rest copy cons) iffalse
) def

a  
  b
    c 
      nil

(l1 l2 append -> l3) doc
(append) ((l1 l2):
  l1 empty? (l2) (l1 first  l1 rest l2 append  cons) ifelse
) def

(append) (
  @1 empty? (@2) (@1 first  @1 rest @2 append  cons) ifelse
) def

(append) (
  over empty? (swap drop) (over first  swap rest swap append  cons) ifelse
) def

list eval

42(x)!!

42(x)!

42(x)set
42(x)setg

42 obj(x)setp
obj(x)getp

list n getat

42 list n setat

42 list cons

list 4 nth

((x 42) (y 44))

"1 2 +" parse -> list

(def reverse (l)
  (cond
    ((empty? l) nil)
    (t (append (reverse (rest l)) (list (first l))))))

(reverse) ((l):
  l empty?
    (nil)
    (l rest reverse  l first list  append)
  ifelse
) def

(reverse) (
  dup empty?
    (dup rest reverse  swap first list  append)
  iffalse
) def

(a b c)
  (b c)
    (c)
      ()
  (c b a)

a b c 
   c b 

a (b c)

(b c) a

*/

//typedef unsigned long VPtr;
typedef unsigned long VFlags;
typedef unsigned long VData;

/*
// 32 bits per item on Arduino
// 128 bits per item on 64 bit machines
typedef struct __VItem
{
  VData            data;  // data + type bit
  struct __VItem*  next;  // next ptr + mark bit
}
VItem;

4 bytes 

512 items
*/

typedef struct __VItem
{
  VFlags           flags; // mark bit, 0 flag = objptr, 1 flag = other type
  VData            data;  // data
  struct __VItem*  next;  // next item
}
VItem;

//#define MALLOC
#define ITEMMEM

#ifdef MALLOC
void MemInit()
{
}
void MemFree()
{
}
VItem* ItemAlloc()
{
  return (VItem*) malloc(sizeof(VItem));
}
void ItemFree(VItem* item)
{
  free(item);
}
#endif

#ifdef ITEMMEM

VItem* ItemMem;
long ItemMemSize = 50000010;
long ItemMemOffset = 0;
VItem* FreeList = NULL;

void MemInit()
{
  ItemMem = malloc(sizeof(VItem) * ItemMemSize);
}

void MemFree()
{
  free(ItemMem);
}

VItem* ItemAlloc()
{
  VItem* item;
  if (FreeList)
  {
    item = FreeList;
    FreeList = item->next;
  }
  else
  {
    item = ItemMem + ItemMemOffset;
    ++ ItemMemOffset;
  }
  return item;
}

void ItemFree(VItem* item)
{
  item->next = FreeList;
  FreeList = item;
}
#endif

void TestItemAlloc()
{
  VItem* item = ItemAlloc();
  item->data = 44;
  printf("item.data: %lu\n", item->data);
  ItemFree(item);
}

VItem* CreateItemList()
{
  VItem* first = ItemAlloc();
  VItem* item = first;
  long i;
  for (i = 0; i < 50000000; ++i)
  {
    item->data = i + 1;
    VItem* next = ItemAlloc();
    item->next = next;
    item = next;
  }
  item->data = i + 1;
  item->next = NULL;

  return first;
}

void PrintItemList(VItem* item)
{
  while (item)
  {
    long n = item->data;
    if (n % 1000000 == 0)
    {
      printf("%lu\n", item->data);
    }
    item = item->next;
  }
}

void DeallocItemList(VItem* item)
{
  while (item)
  {
    VItem* next = item->next;
    ItemFree(item);
    item = next;
  }
}

void TestItemList()
{
  VItem* item = CreateItemList();
  PrintItemList(item);
  DeallocItemList(item);
}

int main()
{
  printf("Hi World\n");
  MemInit();
  TestItemAlloc();
  TestItemList();
  MemFree();
}
