/*
GENERATED FILE.

File: gurumeditation.h
Author: Mikael Kindborg (mikael@kindborg.com)

Usage: php gengurumeditation.php > gurumeditation.h 
*/

#define GURU_MEDITATION 0
#define ITEM_NUMBER_TOO_LARGE 1
#define ITEM_SYMBOL_TOO_LARGE 2
#define ITEM_PRIMFUNID_TOO_LARGE 3
#define ITEM_NOT_NUMBER 4
#define ITEM_NOT_SYMBOL 5
#define ITEM_NOT_PRIMFUN 6
#define ITEM_NOT_POINTER 7
#define ITEM_NOT_STRING 8
#define LISTGROW_OUT_OF_MEMORY 9
#define LISTENSURESIZE_LESS_THAN_ZERO 10
#define LISTGET_INDEX_OUT_OF_BOUNDS 11
#define LISTPOP_CANNOT_POP_EMPTY_LIST 12
#define LISTDROP_CANNOT_DROP_FROM_EMPTY_LIST 13
#define ITEMOBJASLIST_NOT_POINTER 14
#define PARSESTRING_BUFFER_OVERFLOW 15

char* GuruMeditationTable[] = { 
"Guru Meditation"
,"ItemSetNumber: Number is too large"
,"ItemSetSymbol: Symbol id is too large"
,"ItemSetPrimFun: Primfun id is too large"
,"ItemNumber: Not a number"
,"ItemSymbol: Not a symbol!"
,"ItemPrimFun: Not a primfun!"
,"ItemObj: Not a pointer!"
,"ItemString: Not a string!"
,"ListGrow: Out of memory"
,"ListEnsureSize: Index < 0"
,"ListGet: Index out of bounds"
,"ListPop: Cannot pop empty list"
,"ListDrop: Cannot drop from list of length < 0"
,"ItemObjAsList: Not a pointer!"
,"ParseString: Buffer overflow"
}; 
