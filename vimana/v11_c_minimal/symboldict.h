/*
File: symboldict.h
Author: Mikael Kindborg (mikael@kindborg.com)

Dictionary for symbols. Used for parsing and printing.
*/

typedef struct __VSymbolDict
{
  VList* symbols;
  VList* primfuns;
}
VSymbolDict;

// Points to the current symbol dictionary.
VSymbolDict* CurrentSymbolDict = NULL;

#define SymbolDictSymbols(dict)  ((dict)->symbols)
#define SymbolDictPrimFuns(dict) ((dict)->primfuns)

VSymbolDict* SymbolDictCreate()
{
  VSymbolDict* dict = MemAlloc(sizeof(VSymbolDict));
  SymbolDictSymbols(dict) = ItemList_Create();
  SymbolDictPrimFuns(dict) = ItemList_Create();
  return dict;
}

void SymbolDictFree(VSymbolDict* dict)
{
  ListGC(SymbolDictSymbols(dict));
  ListGC(SymbolDictPrimFuns(dict));
  MemFree(dict);
}

char* SymbolDictLookupSymbolName(VSymbolDict* dict, VIndex symbolId)
{
  VItem* item = ItemList_Get(SymbolDictSymbols(dict), symbolId);
  return StringGetStr(ItemString(item));
}

char* SymbolDictLookupPrimFunName(VSymbolDict* dict, VIndex primfunId)
{
  VItem* item = ItemList_Get(SymbolDictPrimFuns(dict), primfunId);
  return StringGetStr(ItemString(item));
}

// Returns -1 if symbol name is not found.
VIndex SymbolDictLookupSymbol(VSymbolDict* dict, char* symbolName)
{
  return ListLookupString(SymbolDictSymbols(dict), symbolName);
}

// Returns -1 if primfun name is not found.
VIndex SymbolDictLookupPrimFun(VSymbolDict* dict, char* primfunName)
{
  return ListLookupString(SymbolDictPrimFuns(dict), primfunName);
}

// Returns index of new item.
VIndex SymbolDictAddSymbol(VSymbolDict* dict, char* symbolName)
{
  return ListAddString(SymbolDictSymbols(dict), symbolName);
}

// Returns index of new item.
VIndex SymbolDictAddPrimFunName(VSymbolDict* dict, char* primfunName)
{
  return ListAddString(SymbolDictPrimFuns(dict), primfunName);
}

// Set a global variable to refer to the current symbol dictionary.
void SymbolDictSetCurrentDict(VSymbolDict* dict)
{
  CurrentSymbolDict = dict;
}

char* SymbolDictCurrentLookupSymbolName(VIndex symbolId)
{
  if (NULL == CurrentSymbolDict)
    GuruMeditation(SYMBOL_DICT_CURRENT_IS_NULL);
  return SymbolDictLookupSymbolName(CurrentSymbolDict, symbolId);
}

char* SymbolDictCurrentLookupPrimFunName(VIndex primFunId)
{
  if (NULL == CurrentSymbolDict)
    GuruMeditation(SYMBOL_DICT_CURRENT_IS_NULL);
  return SymbolDictLookupPrimFunName(CurrentSymbolDict, primFunId);
}

void SymbolDictAddPredefinedSymbols(VSymbolDict* dict)
{
  SymbolDictAddSymbol(dict, "loop");
}
