/*
File: primfuns.h
Author: Mikael Kindborg (mikael@kindborg.com)
*/

void PrimFun_sayHi(VInterp* interp)
{
  PrintLine("Hi World!");
}//

void PrimFun_print(VInterp* interp)
{
  VItem* item = InterpStackPop(interp);
  MemPrintItem(interp->mem, item);
  PrintNewLine();
}//

void PrimFun_printstack(VInterp* interp)
{
  Print("STACK: ");
  MemPrintArray(interp->mem, interp->dataStack, interp->dataStackTop + 1);
  PrintNewLine();
}//

// Eval in current context
void PrimFun_eval(VInterp* interp)
{
  VItem* codeBlock = InterpStackPop(interp);
  InterpPushEvalStackFrame(interp, codeBlock);
}//

// Eval in new context (function call)
void PrimFun_call(VInterp* interp)
{
  VItem* codeBlock = InterpStackPop(interp);
  InterpPushFunCallStackFrame(interp, codeBlock);
}//

// Eval in parent context - this can be used by "macro-like" functions that
// take block arguments (lists) that refer to the context in which the appear
void PrimFun_evalInParentContext(VInterp* interp)
{
  int index;
  VStackFrame* stackframe;

  // Find context of current function call
  for (index = interp->callStackTop; index > -1; --index)
  {
    printf("STACKFRAME: %i\n", index);
    stackframe = InterpStackFrameAt(interp, index);
    if (stackframe->context == stackframe) break;
  }

  // We want the context of the parent of the closest function call
  if (index > 0)
  {
    printf("STACKFRAME 2: %i\n", index - 1);
    stackframe = InterpStackFrameAt(interp, index - 1);
  }

  // Eval in this context
  VItem* codeBlock = InterpStackPop(interp);
  InterpPushStackFrameWithContext(interp, codeBlock, stackframe);  
}//

void PrimFun_iftrue(VInterp* interp)
{
  VItem* trueBlock = InterpStackPop(interp);
  VItem* trueOrFalse = InterpStackPop(interp);
  if (trueOrFalse->intNum)
    InterpPushEvalStackFrame(interp, trueBlock);
}//

void PrimFun_iffalse(VInterp* interp)
{
  VItem* falseBlock = InterpStackPop(interp);
  VItem* trueOrFalse = InterpStackPop(interp);
  if (! trueOrFalse->intNum)
    InterpPushEvalStackFrame(interp, falseBlock);
}//

void PrimFun_ifelse(VInterp* interp)
{
  VItem* falseBlock = InterpStackPop(interp);
  VItem* trueBlock = InterpStackPop(interp);
  VItem* trueOrFalse = InterpStackPop(interp);
  if (trueOrFalse->intNum)
    InterpPushEvalStackFrame(interp, trueBlock);
  else
    InterpPushEvalStackFrame(interp, falseBlock);
}//

void PrimFun_setglobal(VInterp* interp)
{
  VItem* list = InterpStackPop(interp);
  VItem* value = InterpStackPop(interp);
  VItem* symbol = MemItemFirst(interp->mem, list);
  InterpSetGlobalVar(interp, symbol->intNum, value);
}//

void PrimFun_getglobal(VInterp* interp)
{
  VItem* item = InterpStackTop(interp);
  VItem* symbol = MemItemFirst(interp->mem, item);
  *item = *(InterpGetGlobalVar(interp, symbol->intNum));
}//

void PrimFun_funify(VInterp* interp)
{
  VItem* list = InterpStackTop(interp);
  ItemSetType(list, TypeFun);
}//

VItem* ParseSourceCode(char* sourceCode, VMem* mem);

void PrimFun_parse(VInterp* interp)
{
  VItem* item = InterpStackTop(interp);
  // TODO: Check IsTypeString
  char* string = MemBufferItemPtr(interp->mem, item);
  VItem* list = ParseSourceCode(string, interp->mem);
  *item = *list;
}//

void PrimFun_readfile(VInterp* interp)
{
  VItem* stringItem = InterpStackPop(interp);
  // TODO: Check IsTypeString

  char* fileName = MemBufferItemPtr(interp->mem, stringItem);
  char* string = FileRead(fileName);
  // TODO: Check NULL

  stringItem = MemAllocBufferItem(interp->mem, string);
  ItemSetType(stringItem, TypeString);

  InterpStackPush(interp, stringItem);
}//

void PrimFun_plus(VInterp* interp)
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum += b->intNum;
}//

void PrimFun_minus(VInterp* interp)
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum -= b->intNum;
}//

void PrimFun_times(VInterp* interp)
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum *= b->intNum;
}//

void PrimFun_div(VInterp* interp)
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum /= b->intNum;
}//

void PrimFun_1plus(VInterp* interp)
{
  VItem* a = InterpStackTop(interp);
  a->intNum += 1;
}//

void PrimFun_1minus(VInterp* interp)
{
  VItem* a = InterpStackTop(interp);
  a->intNum -= 1;
}//

void PrimFun_2plus(VInterp* interp)
{
  VItem* a = InterpStackTop(interp);
  a->intNum += 2;
}//

void PrimFun_2minus(VInterp* interp)
{
  VItem* a = InterpStackTop(interp);
  a->intNum -= 2;
}//

void PrimFun_lessthan(VInterp* interp)
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum = a->intNum < b->intNum;
}//

void PrimFun_greaterthan(VInterp* interp)
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum = a->intNum > b->intNum;
}//

void PrimFun_eq(VInterp* interp)
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum = ItemEquals(a, b);
  ItemSetType(a, TypeIntNum);
}//

void PrimFun_iszero(VInterp* interp)
{
  VItem* a = InterpStackTop(interp);
  a->intNum = 0 == a->intNum;
}//

void PrimFun_not(VInterp* interp)
{
  VItem* a = InterpStackTop(interp);
  a->intNum = ! a->intNum;
}//

void PrimFun_drop(VInterp* interp)
{
  InterpStackPop(interp);
}//

void PrimFun_dup(VInterp* interp)
{
  VItem* a = InterpStackTop(interp);
  InterpStackPush(interp, a);
}//

void PrimFun_swap(VInterp* interp)
{
  VItem* a = InterpStackAt(interp, 0);
  VItem* b = InterpStackAt(interp, 1);
  VItem temp = *a;
  *a = *b;
  *b = temp;
}//

void PrimFun_over(VInterp* interp)
{
  InterpStackPush(interp, InterpStackAt(interp, 1));
}//

void PrimFun_local_setA(VInterp* interp)
{
  InterpSetLocalVar(interp, 0, InterpStackPop(interp));
}//

void PrimFun_local_setAB(VInterp* interp)
{
  InterpSetLocalVar(interp, 1, InterpStackPop(interp));
  InterpSetLocalVar(interp, 0, InterpStackPop(interp));
}//

void PrimFun_local_setABC(VInterp* interp)
{
  InterpSetLocalVar(interp, 2, InterpStackPop(interp));
  InterpSetLocalVar(interp, 1, InterpStackPop(interp));
  InterpSetLocalVar(interp, 0, InterpStackPop(interp));
}//

void PrimFun_local_setABCD(VInterp* interp)
{
  InterpSetLocalVar(interp, 3, InterpStackPop(interp));
  InterpSetLocalVar(interp, 2, InterpStackPop(interp));
  InterpSetLocalVar(interp, 1, InterpStackPop(interp));
  InterpSetLocalVar(interp, 0, InterpStackPop(interp));
}//

void PrimFun_local_getA(VInterp* interp)
{
  InterpStackPush(interp, InterpGetLocalVar(interp, 0));
}//

void PrimFun_local_getB(VInterp* interp)
{
  InterpStackPush(interp, InterpGetLocalVar(interp, 1));
}//

void PrimFun_local_getC(VInterp* interp)
{
  InterpStackPush(interp, InterpGetLocalVar(interp, 2));
}//

void PrimFun_local_getD(VInterp* interp)
{
  InterpStackPush(interp, InterpGetLocalVar(interp, 3));
}//

// Cool languages:
// https://www.tutorialspoint.com/execute_lisp_online.php
// https://github.com/phreda4/r3d4
// https://concatenative.org/wiki/view/r3
// https://worst.mitten.party

// How other languages do it:
// (cons 1 2) => (1 2)   // NewLisp
// (cons 1 2) => (1 . 2) // Lisp
// (cons 1 nil) => (1)   // Lisp
// (car '()) => nil      // Lisp

//
// Specification of Vimana list functions:
//
// 1 2 cons => error
// 1 () cons => (1)
// 1 (2) cons => (1 2)
// () () cons => (())
// () (1) cons => (() 1)
// () first => ()
// () rest => ()
// () first () cons => (())
// () () eq => 1
// (1) (1) eq => 0
// () isempty => 1
// (isempty) (() eq) def
// (1 2 3) 4 setfirst => (4 2 3)
// () 4 setfirst => (4)
// (1 2 3) (4) setfirst => ((4) 1 2 3)
// (1 2 3) () setfirst => (() 1 2 3)
// (1 2 3) (4) setrest => (1 4)
// (1) () setrest => (1)
// () (1) setrest => (1)
// () () setrest => ()

void PrimFun_first(VInterp* interp)
{
  VItem* list = InterpStackTop(interp);

  // Must be a list type
  if (!IsList(list)) GURU(FIRST_OBJECT_IS_NOT_A_LIST);

  // Get first item
  VItem* item = MemItemFirst(interp->mem, list);

  // Leave empty list on the stack
  // () first => ()
  if (NULL == item) goto Exit;

  // Copy first item to data stack
  *list = *item;
  
Exit:;
}//

void PrimFun_rest(VInterp* interp)
{
  VItem* list = InterpStackTop(interp);

  // Must be a list type
  if (!IsList(list)) GURU(REST_OBJECT_IS_NOT_A_LIST);

  // Get first item
  VItem* item = MemItemFirst(interp->mem, list);

  // Leave empty list on the stack
  // () rest => ()
  if (NULL == item) goto Exit;

  // Get second item in the list
  item = MemItemNext(interp->mem, item);

  // If empty tail, leave empty list on the stack
  // (1) rest => ()
  if (NULL == item)
  {
    list->addr = 0;
    goto Exit;
  }

  // Set second item as first element of the list
  MemItemSetFirst(interp->mem, list, item);

Exit:;
}//

void PrimFun_cons(VInterp* interp)
{
  // Get list and item to cons
  VItem* list = InterpStackPop(interp);
  VItem* item = InterpStackTop(interp);

  // Must be a list type
  if (!IsList(list)) GURU(CONS_OBJECT_IS_NOT_A_LIST);

  // This will be the new head of the cons
  VItem newList;
  ItemInit(&newList);
  ItemSetType(&newList, ItemType(list));

  // Allocate new element
  VItem* newFirst = MemAllocItem(interp->mem);
  if (NULL == newFirst) GURU(CONS_OUT_OF_MEMORY);

  // Copy item to new element
  *newFirst = *item;

  // Get first element of the list 
  VItem* first = MemItemFirst(interp->mem, list);

  if (NULL == first)
  {
    // If empty list, the new item is the last and only element
    MemItemSetNext(interp->mem, newFirst, NULL);
  }
  else
  {
    // Link new item to the first element of the list
    MemItemSetNext(interp->mem, newFirst, first);
  }

  // Set first of list to refer to the new element
  MemItemSetFirst(interp->mem, &newList, newFirst);

  // Copy new list item to data stack
  *item = newList;
}//

// Leaves list on the stack
// list item setfirst --> list
void PrimFun_setfirst(VInterp* interp)
{
  VItem* item = InterpStackPop(interp);
  VItem* list = InterpStackTop(interp);

  // Must be a list type
  if (!IsList(list)) GURU(SETFIRST_OBJECT_IS_NOT_A_LIST);

  // Get first item
  VItem* first = MemItemFirst(interp->mem, list);

  // Set first of empty list
  if (NULL == first)
  {
    first = MemAllocItem(interp->mem);
    if (NULL == first) GURU(SETFIRST_OUT_OF_MEMORY);
    MemItemSetFirst(interp->mem, list, first);
  }

  // Preserve address of next item
  VAddr next = first->next;
  // Copy item
  *first = *item;
  // Restore next
  first->next = next;
}//

void PrimFun_gc(VInterp* interp)
{
  InterpGC(interp);
}//

// millis --> millisecond time stamp
void PrimFun_millis(VInterp* interp)
{
  struct timeval timestamp;
  VItem item;

  gettimeofday(&timestamp, NULL);
  long days = (1000 * 60 * 24 * 1000);
  long millis = 
    ((timestamp.tv_sec % days) * 1000) + 
    (timestamp.tv_usec / 1000);
  //printf("sec: %li millis: %li\n", (long)timestamp.tv_sec % days, (long)timestamp.tv_usec / 1000);
  ItemSetIntNum(&item, millis);

  InterpStackPush(interp, &item);
}//

// millis sleep -->
void PrimFun_sleep(VInterp* interp)
{
  VItem* item = InterpStackPop(interp);
  if (!IsTypeIntNum(item))
  {
    GURU(SLEEP_NOT_INTNUM);
  }

  int millis = item->intNum;
  int seconds = millis / 1000;
  int micros = (millis % 1000) * 1000;

  sleep(seconds);
  usleep(micros);
}//

/*
// port socketcreate --> socket
void PrimFunx_socketcreate(VInterp* interp)
{
  VItem* port = InterpStackPop(interp);

  int socketId = socket(AF_INET, SOCK_STREAM, 0);
  if (socketId < 1)
  {
    GURU(SOCKET_CREATE_ERROR);
  }

  int status = fcntl(socketId, F_SETFL, fcntl(socketId, F_GETFL, 0) | O_NONBLOCK);
  if (status == -1)
  {
    GURU(SOCKET_CREATE_FCNTL_ERROR);
  }
  
  VSocket* socket = SysAlloc(sizeof(VSocket));

  socket->socketId = socketId;
  socket->address.sin_family = AF_INET;
  socket->address.sin_addr.s_addr = htonl(INADDR_ANY);
  socket->address.sin_port = htons(port->intNum);

  status = bind(socketId, (struct sockaddr *) & (socket->address), sizeof(struct sockaddr_in));
  if (status < 0)
  {
    SysFree(socket);
    GURU(SOCKET_BIND_ERROR);
  }

  VItem socketItem;

  ItemSetType(&socketItem, TypeBufferPtr);
  socketItem.ptr = socket;

  InterpStackPush(interp, &socketItem);
}//

// socket socketlisten --> socket
void PrimFunx_socketlisten(VInterp* interp)
{
  VItem* socket = InterpStackTop(interp);

  int status = listen(socket->intNum, 10);
  if (status < 0) 
  {
    GURU(SOCKET_LISTEN_ERROR);
  }
}//

// socket socketaccept --> socket result (1 if data available, 0 if not)
void PrimFunx_socketaccept(VInterp* interp)
{
  VItem result;

  VItem* socket = InterpStackTop(interp);

  socklen_t addrlen;
  int clientSocketId = accept(socket->socketId, (struct sockaddr *) & (socket->address), &addrlen);
  //printf("clientSocketId: %i\n", clientSocketId);
  if (clientSocketId < 0)
  {
    if (EWOULDBLOCK == errno) 
    {
      ItemSetIntNum(&result, 0);
    } 
    else 
    {
      GURU(SOCKET_ACCEPT_ERROR);
    }
  }
  else
  {
    ItemSetIntNum(&result, 1);
  }

  InterpStackPush(&result);
}//

// socket socketaccept --> socket result (1 if data available, 0 if not)
void PrimFunx_socketaccept(VInterp* interp)
{
  VItem result;

  VItem* socket = InterpStackTop(interp);

  socklen_t addrlen;
  int clientSocketId = accept(socket->socketId, (struct sockaddr *) & (socket->address), &addrlen);
  //printf("clientSocketId: %i\n", clientSocketId);
  if (clientSocketId < 0)
  {
    if (EWOULDBLOCK == errno) 
    {
      ItemSetIntNum(&result, 0);
    } 
    else 
    {
      GURU(SOCKET_ACCEPT_ERROR);
    }
  }
  else
  {
    ItemSetIntNum(&result, 1);
  }

  InterpStackPush(&result);
}//


// socket socketclose --> 
void PrimFunx_sockeclose(VInterp* interp)
{
  VItem* socket = InterpStackPop(interp);
close(client_socket);
}//
*/

void PrimFun_def(VInterp* interp)
{
  PrimFun_funify(interp);
  PrimFun_swap(interp);
  PrimFun_setglobal(interp);
}//

void PrimFun_evalfile(VInterp* interp)
{
  PrimFun_readfile(interp);
  PrimFun_parse(interp);
  PrimFun_eval(interp);
}//

typedef struct __PrimFunEntry
{
  char*       name;
  VPrimFunPtr fun;
}
PrimFunEntry;

PrimFunEntry GPrimFunTable[] = 
{
  #include "primfuntable.h"
};

int LookupPrimFun(char* name)
{
  for (int i = 0; ; ++ i)
  {
    PrimFunEntry* entry = & (GPrimFunTable[i]);
    if (StrEquals(entry->name, name))
      return i;
    else
    if (NULL == entry->fun)
      return -1;
  } 
}

VPrimFunPtr LookupPrimFunPtr(int index)
{
  return GPrimFunTable[index].fun;
}
