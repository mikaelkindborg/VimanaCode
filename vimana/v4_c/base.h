
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef unsigned char Type;
typedef int Index;
typedef int Bool;

typedef struct MyList List;
typedef struct MyItem Item;
typedef struct MyInterp Interp;
typedef void (*PrimFun)(Interp*);

#define FALSE 0
#define TRUE  1

#define TypeSymbol  1
#define TypePrimFun 2
#define TypeFun     3
#define TypeIntNum  4
#define TypeList    5
#define TypeObj     6
#define TypeString  7
#define TypeDecNum  8
#define TypeVirgin  9

#define IsSymbol(type)  ((type) == (TypeSymbol))
#define IsPrimFun(type) ((type) == (TypePrimFun))
#define IsFun(type)     ((type) == (TypeFun))
#define IsIntNum(type)  ((type) == (TypeIntNum))
#define IsDecNum(type)  ((type) == (TypeDecNum))
#define IsList(type)    ((type) == (TypeList))
#define IsObj(type)     ((type) == (TypeObj))
#define IsString(type)  ((type) == (TypeString))
#define IsVirgin(type)  ((type) == (TypeVirgin))

#define StringEquals(s1, s2) (0 == strcmp((s1), (s2)))

#define ListFreeShallow 1
#define ListFreeDeep    2
#define ListFreeDeeper  3
