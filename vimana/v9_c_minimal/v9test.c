#include "base.h"
#include "gurumeditation_gen.h"
#include "item.h"
#include "list.h"
#include "itemlist_gen.h"
#include "itemprint.h"

void TestList()
{
  VList* list = ItemList_Create();
  VItem item;
  ItemSetNumber(&item, 32);
  ItemList_Push(list, &item);
  ItemList_Push(list, &item);
  ItemList_Push(list, &item);
  ItemList_Push(list, &item);
  ItemList_Push(list, &item);
  ItemList_Push(list, &item);
  PrintList(list);
  PrintNewLine();
  while (ListLength(list) > 0) 
    PrintStrNumLine("item: ", ItemNumber(ItemList_Pop(list)));
  ListFree(list);
}

// MAIN --------------------------------------------------------

int main(int numargs, char* args[])
{
  TestList();
  PrintMemStat();
}
