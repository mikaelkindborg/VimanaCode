/*
File: symboldict.h
Author: Mikael Kindborg (mikael@kindborg.com)

Dictionary for symbols. Used for parsing and printing.
*/

typedef struct __VSymbolDict
{
  VList symbols;
  VList primfuns;
}
VSymbolDict;

#define SymbolDictSymbols(dict)  (& ((dict)->symbols))
#define SymbolDictPrimFuns(dict) (& ((dict)->primfuns))

VSymbolDict* SymbolDictCreate()
{
  VSymbolDict* dict = MemAlloc(sizeof(VSymbolDict));
  ListInit(SymbolDictSymbols(dict), sizeof(VItem));
  ListInit(SymbolDictPrimFuns(dict), sizeof(VItem));
  return dict;
}

void SymbolDictFree(VSymbolDict* dict)
{
  ListDeallocArrayBufDeep(SymbolDictSymbols(dict));
  ListDeallocArrayBufDeep(SymbolDictPrimFuns(dict));
  MemFree(dict);
}

char* SymbolDictLookupSymbolId(VSymbolDict* dict, VIndex symbolId)
{
  VItem* item = ListGet(SymbolDictSymbols(dict), symbolId);
  return StringGetStr(ItemObj(item));
}

char* SymbolDictLookupPrimFunId(VSymbolDict* dict, VIndex primfunId)
{
  VItem* item = ListGet(SymbolDictPrimFuns(dict), primfunId);
  return StringGetStr(ItemObj(item));
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
