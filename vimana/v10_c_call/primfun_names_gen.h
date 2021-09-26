/*
File: primfun_names_gen.h

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
  SymbolDictAddPrimFunName(dict, "eq");
  SymbolDictAddPrimFunName(dict, "<");
  SymbolDictAddPrimFunName(dict, ">");
  SymbolDictAddPrimFunName(dict, "iszero");
  SymbolDictAddPrimFunName(dict, "eval");
  SymbolDictAddPrimFunName(dict, "iftrue");
  SymbolDictAddPrimFunName(dict, "iffalse");
  SymbolDictAddPrimFunName(dict, "ifelse");
  SymbolDictAddPrimFunName(dict, "not");
  SymbolDictAddPrimFunName(dict, "drop");
  SymbolDictAddPrimFunName(dict, "dup");
  SymbolDictAddPrimFunName(dict, "2dup");
  SymbolDictAddPrimFunName(dict, "over");
  SymbolDictAddPrimFunName(dict, "swap");
  SymbolDictAddPrimFunName(dict, "printstack");
}
