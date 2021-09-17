/*
File: symboldict_primfuns.h

This is file is generate by genprimfuns.php
*/

void SymbolDictAddPrimFuns(VSymbolDict* dict)
{
  SymbolDictAddPrimFunName(dict, "print");
  SymbolDictAddPrimFunName(dict, "setglobal");
  SymbolDictAddPrimFunName(dict, "def");
  SymbolDictAddPrimFunName(dict, "+");
  SymbolDictAddPrimFunName(dict, "-");
  SymbolDictAddPrimFunName(dict, "*");
  SymbolDictAddPrimFunName(dict, "/");
}
