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
#define ITEM_NOT_BOOL 5
#define ITEM_NOT_SYMBOL 6
#define ITEM_NOT_PRIMFUN 7
#define ITEM_NOT_POINTER 8
#define ITEM_NOT_STRING 9
#define LISTGROW_OUT_OF_MEMORY 10
#define LISTGROW_VINDEXMAX_EXCEEDED 11
#define LISTCHECKCAPACITY_LESS_THAN_ZERO 12
#define LISTCHECKCAPACITY_VINDEXMAX_EXCEEDED 13
#define LISTGET_INDEX_OUT_OF_BOUNDS 14
#define LISTPOP_CANNOT_POP_EMPTY_LIST 15
#define LISTDROP_CANNOT_DROP_FROM_EMPTY_LIST 16
#define ITEMOBJASLIST_NOT_POINTER 17
#define PARSESTRING_BUFFER_OVERFLOW 18
#define PARSETOKEN_BUFFER_OVERFLOW 19
#define SYMBOL_UNKNOWN_TYPE 20
#define OBJGC_UNKNOWN_TYPE 21

#ifdef GURUMEDITATION_STRINGS
char* GuruMeditationTable[] = { 
"Guru Meditation"
,"ItemSetNumber: Number is too large"
,"ItemSetSymbol: Symbol id is too large"
,"ItemSetPrimFun: Primfun id is too large"
,"ItemNumber: Not a number"
,"ItemBool: Not a boolean"
,"ItemSymbol: Not a symbol!"
,"ItemPrimFun: Not a primfun!"
,"ItemObj: Not a pointer!"
,"ItemString: Not a string!"
,"ListGrow: Out of memory"
,"ListGrow: VINDEXMAX exceeded"
,"ListCheckCapacity: Index < 0"
,"ListCheckCapacity: VINDEXMAX exceeded"
,"ListGet: Index out of bounds"
,"ListPop: Cannot pop empty list"
,"ListDrop: Cannot drop from list of length < 0"
,"ItemObjAsList: Not a pointer!"
,"ParseString: Buffer overflow"
,"ParseToken: Buffer overflow"
,"Print: Unknown symbol type"
,"ObjGC: ALERT - UNKNOWN TYPE"
}; 
#endif
