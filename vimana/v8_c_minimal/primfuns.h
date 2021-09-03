
switch (primFun)
{
  VmItem* item;
  
  case 1: // print
    item = InterpPop(interp);
    PrintNum(ItemNumber(*item)); 
    PrintNewLine();
    break;
}
