/*
File: item.h
Author: Mikael Kindborg (mikael@kindborg.com)

Items are objects that hold values. They are used in lists and on the data stack.
*/

/*

InterpRun -> InterpEval
- do not free code list automatically

InterpCallFun(VInterp* interp, VIndex symbolId)
- Use predefined symbols, like loop (like primfuns)

Schedule(VScheduler* scheduler, VInterp* interp)

InterpEval(VInterp* interp, VList* code)

InterpEvalPrepare/Setup(VInterp* interp, VList* code)
InterpEvalSlice(VInterp* interp, VNumber numSteps)
InterpClearStack(VInterp* interp)

Primfun: millis suspend

*/

/*
Alternative representations for VItem
-------------------------------------

Currently a pointer-sized VItem is used (16 bits on Arduino and 64 bits on macOS)

Alternatively, use long on Arduino (32 bits) instead of int/pointer (16 bits)
- Can hold 31 bit integers (one type bit is used for tagging integers)
- Can hold 16-bit pointers
- Structs need not be 16 bit aligned
- Can hold pointers to primitive functions (table with function pointers take up heap space, but the table should be possible to do as code)
- Can not hold floats
- Takes up more space (x2)

As a variation of the above, use two fields (pointer, value): 
- 32 bits on Ardino, 128 bits on 64 bit machines
- Can hold 16-bit pointers (Arduino)
- Structs need not be 16 bit aligned (Arduino)
- The pointer field can be a function pointer for interpreter dispatch of operations on item types and for primitive functions (this means zero branching in the main interpreter loop)
- Can not hold floats
- On Arduino, only 16 bit integers can be used without branching on the int type bit
- Takes up more space (x2)

Using sequences of items of varying size is another possibility, but this breaks the principle of arrays of code that can be manipulated as data (lists).

Test on Arduino Uno 210922
--------------------------

This is with 16 bit VItem

The Vimana code computes 32 fib 5 times

12:38:17.140 -> BEGIN
12:38:18.142 -> sizeof(VItem)2
12:38:18.142 -> sizeof(VList)8
12:38:18.179 -> sizeof(VContext)4
12:38:18.179 -> sizeof(VInterp)12
12:38:18.213 -> sizeof(VObj)2
12:38:18.213 -> sizeof(void*)2
12:38:18.249 -> sizeof(int)2
12:38:18.249 -> sizeof(long)4
12:38:18.283 -> sizeof(double)4
12:38:18.283 -> sizeof(float)4
12:38:18.283 -> 1596 (free RAM before parsing symbolic code)
12:38:18.321 -> (L(S0) L(P17 2 P8 L(P17 1 P4 S0 P20 2 P4 S0 P3) P13) P2 L(S1) L(P17 P10 L(P16 P16) L(P20 P17 P11 P20 1 P4 S1) P14) P2 L(18 S0 P0) 5 S1)
12:38:18.462 -> 1294 (free RAM after parsing symbolic code)
12:38:21.605 -> 2584 (18 fib)
12:38:24.856 -> 2584 (18 fib)
12:38:28.075 -> 2584 (18 fib)
12:38:31.332 -> 2584 (18 fib)
12:38:34.558 -> 2584 (18 fib)
12:38:34.558 -> 946 (free RAM after exit of InterpRun)
12:38:34.558 -> 1596 (free RAM after InterpFree)
*/

// STRUCTS -----------------------------------------------------

// Data item used in lists
typedef struct __VItem
{
  union
  {
    VObj*     obj;
    VNumber   number;
    VUNumber  bits;
  }
  value;
}
VItem;

// ITEM TYPES --------------------------------------------------

// Three low bits are used for type information.
// Pointers have    0 in the low bit (type VObj, VList, VString)
// Numbers  have   01 in the low bits
// Symbols  have  011 in the low bits
// Primfun  have  111 in the low bits

#define TypeBitMask1      1
#define TypeBitMask2      3
#define TypeBitMask3      7
#define TypeObj           0 // Use TypeBitMask1
#define TypeNumber        1 // Use TypeBitMask2
#define TypeSymbol        3 // Use TypeBitMask3
#define TypePrimFun       7 // Use TypeBitMask3

// ITEM TYPE CHECKING ------------------------------------------

#define IsVirgin(item)    ((item)->value.bits == 0)
#define IsObj(item)       (((item)->value.bits & TypeBitMask1) == TypeObj)
#define IsNumber(item)    (((item)->value.bits & TypeBitMask2) == TypeNumber)
#define IsBool(item)      IsNumber(item)
#define IsSymbol(item)    (((item)->value.bits & TypeBitMask3) == TypeSymbol)
#define IsPrimFun(item)   (((item)->value.bits & TypeBitMask3) == TypePrimFun)
#define IsList(item) \
  ( (IsObj(item)) && (ObjIsList(ItemObj(item))) )
#define IsFun(item) \
  ( (IsObj(item)) && (ObjIsFun(ItemObj(item))) )
#define IsString(item) \
  ( (IsObj(item)) && (ObjIsString(ItemObj(item))) )

// SET ITEMS ------------------------------------------------

#define ItemSetList(item, list)     ItemSetObj(item, (VObj*)list)
#define ItemSetString(item, string) ItemSetObj(item, (VObj*)string)
#define ItemSetBool(item, boolean)  ItemSetNumber(item, boolean)

// Create uninitialized value
void ItemSetVirgin(VItem* item)
{
  item->value.bits = 0;
}

// The item takes ownership of the object.
void ItemSetObj(VItem* item, VObj* obj)
{
  item->value.obj = obj;
}

void ItemSetNumber(VItem* item, VNumber number)
{
  if (((number << 2) >> 2) != number) 
    GuruMeditation(ITEM_NUMBER_TOO_LARGE);
  item->value.number = (number << 2) | TypeNumber;
}

void ItemSetSymbol(VItem* item, VNumber symbolId)
{
  if (((symbolId << 3) >> 3) != symbolId) 
    GuruMeditation(ITEM_SYMBOL_TOO_LARGE);
  item->value.number = (symbolId << 3) | TypeSymbol;
}

void ItemSetPrimFun(VItem* item, VNumber primFunId)
{
  if (((primFunId << 3) >> 3) != primFunId) 
    GuruMeditation(ITEM_PRIMFUNID_TOO_LARGE);
  item->value.number = (primFunId << 3) | TypePrimFun;
}

// ACCESS ITEMS ------------------------------------------------

#define ItemList(item)   ((VList*)ItemObj(item))
#define ItemString(item) ((VString*)ItemObj(item))
#define ItemBool(item)   ItemNumber(item)

VNumber ItemNumber(VItem* item)
{
  if (!IsNumber(item)) 
    GuruMeditation(ITEM_NOT_NUMBER);
  return item->value.number >> 2;
}

VNumber ItemSymbol(VItem* item)
{
  if (!IsSymbol(item)) 
    GuruMeditation(ITEM_NOT_SYMBOL);
  return item->value.number >> 3;
}

VNumber ItemPrimFun(VItem* item)
{
  if (!IsPrimFun(item)) 
    GuruMeditation(ITEM_NOT_PRIMFUN);
  return item->value.number >> 3;
}

VObj* ItemObj(VItem* item)
{
  if (!IsObj(item)) 
    GuruMeditation(ITEM_NOT_POINTER);
  return item->value.obj;
}

// COMPARE ITEMS -----------------------------------------------

#define ItemEquals(item1, item2) \
  ( (((VItem*)(item1))->value.bits) == (((VItem*)(item2))->value.bits) )
