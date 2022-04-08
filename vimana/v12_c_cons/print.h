/*
File: print.h
Author: Mikael Kindborg (mikael@kindborg.com)

Print functions.
*/

void PrintBinaryUInt(unsigned int n)
{
  int numBits = sizeof(unsigned int) * 8;
  for (long i = numBits - 1 ; i >= 0; --i) 
  {
    PrintChar(n & (1L << i) ? '1' : '0');
  }
  PrintNewLine();
}

void PrintList(VItem* first, VMem* mem);

void PrintItem(VItem* item, VMem* mem)
{
  //printf("type: %i ", ItemType(item));
  if (TypeList == ItemType(item))
    PrintList(MemItemPointer(mem, ItemData(item)), mem);
  else if (TypeIntNum == ItemType(item))
    printf("N%i", (int)ItemData(item));
  else if (TypePrimFun == ItemType(item))
    printf("P%i", (int)ItemData(item));
}

void PrintList(VItem* first, VMem* mem)
{
  printf("(");
  VAddr addr = MemItemAddr(mem, first);
  int printSpace = FALSE;
  while (addr)
  {
    if (printSpace) printf(" ");
    VItem* item = MemItemPointer(mem, addr);
    PrintItem(item, mem);
    addr = ItemNext(item);
    printSpace = TRUE;
  }
  printf(")");
}
