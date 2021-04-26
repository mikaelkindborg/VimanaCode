
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef unsigned char Type;
typedef int Index;
typedef int Bool;

typedef struct MyList List;
typedef struct MyItem Item;
typedef struct MyInterp Interp;

#define FALSE 0
#define TRUE  1

#define TypeSymbol 1
#define TypePrim   2
#define TypeFun    3
#define TypeNumber 4
#define TypeList   5
#define TypeObj    6

#define IsSymbol(type) ((type) == (TypeSymbol))
#define IsPrim(type)   ((type) == (TypePrim))
#define IsFun(type)    ((type) == (TypeFun))
#define IsNumber(type) ((type) == (TypeNumber))
#define IsList(type)   ((type) == (TypeList))
#define IsObj(type)    ((type) == (TypeObj))

#define StringEquals(s1, s2) (0 == strcmp((s1), (s2)))

#define ListFreeShallow 1
#define ListFreeDeep    2
#define ListFreeDeeper  3

char* InterpGetSymbol(Interp* interp, Index symbolIndex);
