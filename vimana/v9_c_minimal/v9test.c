#include "base.h"
#include "gurumeditation_gen.h"
#include "item.h"
#include "list.h"
#include "string.h"
#include "itemlist.h"
#include "itemprint.h"
#include "interp.h"
#include "codeparser.h"
#include "symboldict.h"
#include "primfunsdict_gen.h"
#include "sourceparser.h"

void TestList()
{
  PrintLine("--- TestList ---");
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

void TestList2()
{
  PrintLine("--- TestList2 ---");
  VList* list = ItemList_Create();
  VItem* item;
  item = ListPushRaw(list);
  ItemSetNumber(item, 32);
  item = ListPushRaw(list);
  ItemSetNumber(item, 64);
  item = ListPushRaw(list);
  ItemSetNumber(item, 128);
  PrintList(list);
  PrintNewLine();
  while (ListLength(list) > 0) 
    PrintStrNumLine("item: ", ItemNumber(ItemList_Pop(list)));
  ListFree(list);
}

void TestStackOperations()
{
  PrintLine("--- TestStackOperations ---");

  VList* list = ItemList_Create();
  VItem item;
  ItemSetNumber(&item, 32);
  ItemList_Push(list, &item);
  ItemSetNumber(&item, 16);
  ItemList_Push(list, &item);

  PrintLine("LIST:");
  PrintList(list);
  PrintNewLine();

  PrintLine("SWAP:");
  ItemListSwap(list);
  PrintList(list);
  PrintNewLine();
  
  PrintLine("2DUP:");
  ItemList2Dup(list);
  PrintList(list);
  PrintNewLine();
  
  PrintLine("OVER:");
  ItemListOver(list);
  PrintList(list);
  PrintNewLine();
  
  PrintLine("DUP:");
  ItemListDup(list);
  PrintList(list);
  PrintNewLine();

  ListFree(list);
}

void TestSymbolicParser()
{
  PrintLine("--- TestSymbolicParser ---");

  VList* codeList = ParseSymbolicCode("N8888881 P0 N33 N33 P3 P0 (S1 S2) P0 ('FOO HEJ HOPP') P0");
  PrintList(codeList);
  PrintNewLine();
  ListGC(codeList);
}

void TestInterpreter3()
{
  PrintLine("--- TestInterpreter3 ---");

  VList* code;

  VInterp* interp = InterpCreate();
  VSymbolDict* dict = SymbolDictCreate();
  SymbolDictAddPrimFuns(dict);

  code = ParseSourceCode("1 2 + print", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);

  code = ParseSourceCode("42 42", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);

  code = ParseSourceCode("* print", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);

  code = ParseSourceCode("'HI WORLD' print", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);

  code = ParseSourceCode("(foo) ('HI AGAIN' print) def foo foo", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);

  code = ParseSourceCode("(double) (2 *) def 4 double print", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);

  code = ParseSourceCode("'I am Foobar' (foobar) setglobal", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);

  // WORKS with refcount gc.
  // This should crash without refcount or gc.
  code = ParseSourceCode("foobar foobar print", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);

  // WORKS with refcount gc.
  // This should crash without refcount or gc.
  code = ParseSourceCode("foobar (foobar2) setglobal foobar2 print", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);

  // This should work.
  code = ParseSourceCode("foobar print", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);

  SymbolDictFree(dict);
  InterpFree(interp);
}

// MAIN --------------------------------------------------------

int main(int numargs, char* args[])
{
  TestList();
  TestList2();
  TestStackOperations();
  TestSymbolicParser();
  TestInterpreter3();
  PrintMemStat();
}
