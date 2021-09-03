
switch (primFun)
{
  VmItem* item;
  
  case 1: // print
    item = InterpPop(interp);
    PrintNum(item->value.number); 
    PrintNewLine();
    break;
}
