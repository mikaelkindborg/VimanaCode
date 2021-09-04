
switch (primFun)
{
  VmItem* item1;
  VmItem* item2;
  
  case 1: // print
    item1 = InterpPop(interp);
    PrintItem(item1);
    //PrintNum(ItemNumber(*item1)); 
    PrintNewLine();
    break;
  case 2: // add
    item2 = InterpPop(interp);
    item1 = InterpPop(interp);
    ItemSetNumber(item1, ItemNumber(*item1) + ItemNumber(*item2));
    InterpPush(interp, item1);
    break;
}
