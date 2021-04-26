
/****************** PRIMS ******************/

void InterpAddPrimFun(char* name, PrimFun fun, Interp* interp)
{
  // Add to symbol table.
  List* symbolTable = interp->symbolTable;
  Item item = ItemWithString(name);
  item.type = TypePrimFun;
  item.data.primFun = fun;
  item.symbolIndex = -1;
  Index index = ListPush(symbolTable, item);
}

void Prim_PRINTLN(Interp* interp)
{
  printf("HELLO PRINTLN\n");
}

void InterpDefinePrimFuns(Interp* interp)
{
  InterpAddPrimFun("PRINTLN", &Prim_PRINTLN, interp);
}

