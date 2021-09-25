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
#ifdef GC_REFCOUNT
  ListGC(SymbolDictSymbols(dict));
  ListGC(SymbolDictPrimFuns(dict));
#endif
  MemFree(dict);
}

char* SymbolDictLookupSymbolId(VSymbolDict* dict, VIndex symbolId)
{
  VItem* item = ItemList_Get(SymbolDictSymbols(dict), symbolId);
  return StringGetStr(ItemString(item));
}

char* SymbolDictLookupPrimFunId(VSymbolDict* dict, VIndex primfunId)
{
  VItem* item = ItemList_Get(SymbolDictPrimFuns(dict), primfunId);
  return StringGetStr(ItemString(item));
}

// Returns -1 if symbol name is not found.
VIndex SymbolDictLookupSymbolName(VSymbolDict* dict, char* symbolName)
{
  return ListLookupString(SymbolDictSymbols(dict), symbolName);
}

// Returns -1 if primfun name is not found.
VIndex SymbolDictLookupPrimFunName(VSymbolDict* dict, char* primfunName)
{
  return ListLookupString(SymbolDictPrimFuns(dict), primfunName);
}

// Returns index of new item.
VIndex SymbolDictAddSymbolName(VSymbolDict* dict, char* symbolName)
{
  return ListAddString(SymbolDictSymbols(dict), symbolName);
}

// Returns index of new item.
VIndex SymbolDictAddPrimFunName(VSymbolDict* dict, char* primfunName)
{
  return ListAddString(SymbolDictPrimFuns(dict), primfunName);
}
