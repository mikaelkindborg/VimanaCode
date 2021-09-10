/*
File: symboldictfuns.h

This is file is generate by genprimfuns.php
*/

void SymbolDictAddPrimFuns(VSymbolDict* dict)
{
  SymbolDictAddPrimFunName(dict, "print");
  SymbolDictAddPrimFunName(dict, "setglobal");
  SymbolDictAddPrimFunName(dict, "def");
  SymbolDictAddPrimFunName(dict, "add");
  SymbolDictAddPrimFunName(dict, "sub");
  SymbolDictAddPrimFunName(dict, "mult");
  SymbolDictAddPrimFunName(dict, "div");
}
