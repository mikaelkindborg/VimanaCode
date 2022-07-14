/*
File: primfuns.h
Author: Mikael Kindborg (mikael@kindborg.com)
*/

void PrimFun_sayHi(VInterp* interp)
{
  PrintLine("Hi World!");
}

void PrimFun_print(VInterp* interp)
{
  VItem* item = InterpStackPop(interp);
  InterpPrint(interp, item);
  PrintNewLine();
}

void PrimFun_printstack(VInterp* interp)
{
  Print("STACK: ");
  PrintItemArray(interp->dataStack, interp->dataStackTop + 1, interp);
  PrintNewLine();
}

void PrimFun_eval(VInterp* interp)
{
  VItem* codeBlock = InterpStackPop(interp);
  InterpPushStackFrame(interp, codeBlock);
}

void PrimFun_iftrue(VInterp* interp)
{
  VItem* trueBlock = InterpStackPop(interp);
  VItem* trueOrFalse = InterpStackPop(interp);
  if (trueOrFalse->intNum)
    InterpPushStackFrame(interp, trueBlock);
}

void PrimFun_iffalse(VInterp* interp)
{
  VItem* falseBlock = InterpStackPop(interp);
  VItem* trueOrFalse = InterpStackPop(interp);
  if (! trueOrFalse->intNum)
    InterpPushStackFrame(interp, falseBlock);
}

void PrimFun_ifelse(VInterp* interp)
{
  VItem* falseBlock = InterpStackPop(interp);
  VItem* trueBlock = InterpStackPop(interp);
  VItem* trueOrFalse = InterpStackPop(interp);
  if (trueOrFalse->intNum)
    InterpPushStackFrame(interp, trueBlock);
  else
    InterpPushStackFrame(interp, falseBlock);
}

void PrimFun_setglobal(VInterp* interp)
{
  VItem* list = InterpStackPop(interp);
  VItem* value = InterpStackPop(interp);
  VItem* symbol = GetFirst(list, interp);
  InterpSetGlobalVar(interp, symbol->intNum, value);
}

void PrimFun_getglobal(VInterp* interp)
{
  VItem* item = InterpStackTop(interp);
  VItem* symbol = GetFirst(item, interp);
  *item = *(InterpGetGlobalVar(interp, symbol->intNum));
}

void PrimFun_funify(VInterp* interp)
{
  VItem* list = InterpStackTop(interp);
  ItemSetType(list, TypeFun);
}

void PrimFun_parse(VInterp* interp)
{
  VItem* item = InterpStackTop(interp);
  if (!IsTypeString(item)) GURU_MEDITATION(PARSE_ARG_NOT_STRING);
  char* string = GetHandlePtr(item, interp);
  VItem* list = InterpParse(interp, string);
  *item = *list;
}

void PrimFun_readfile(VInterp* interp)
{
  VItem* item = InterpStackPop(interp);
  if (!IsTypeString(item)) GURU_MEDITATION(READFILE_ARG_NOT_STRING);

  char* fileName = GetHandlePtr(item, interp);
  char* data = FileRead(fileName);

  if (NULL != data)
  {
    item = AllocHandle(data, TypeString, interp);
    InterpStackPush(interp, item);
  }
  // else ??
}

void PrimFun_plus(VInterp* interp) // +
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum += b->intNum;
}

void PrimFun_minus(VInterp* interp) // -
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum -= b->intNum;
}

void PrimFun_times(VInterp* interp) // *
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum *= b->intNum;
}

void PrimFun_div(VInterp* interp) // /
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum /= b->intNum;
}

void PrimFun_1plus(VInterp* interp) // 1+
{
  VItem* a = InterpStackTop(interp);
  a->intNum += 1;
}

void PrimFun_1minus(VInterp* interp) // 1-
{
  VItem* a = InterpStackTop(interp);
  a->intNum -= 1;
}

void PrimFun_2plus(VInterp* interp) // 2+
{
  VItem* a = InterpStackTop(interp);
  a->intNum += 2;
}

void PrimFun_2minus(VInterp* interp) // 2-
{
  VItem* a = InterpStackTop(interp);
  a->intNum -= 2;
}

void PrimFun_lessthan(VInterp* interp) // <
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum = a->intNum < b->intNum;
}

void PrimFun_greaterthan(VInterp* interp) // >
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum = a->intNum > b->intNum;
}

void PrimFun_eq(VInterp* interp)
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum = ItemEquals(a, b);
  ItemSetType(a, TypeIntNum);
}

void PrimFun_iszero(VInterp* interp)
{
  VItem* a = InterpStackTop(interp);
  a->intNum = 0 == a->intNum;
}

void PrimFun_not(VInterp* interp)
{
  VItem* a = InterpStackTop(interp);
  a->intNum = ! a->intNum;
}

void PrimFun_drop(VInterp* interp)
{
  InterpStackPop(interp);
}

void PrimFun_dup(VInterp* interp)
{
  VItem* a = InterpStackTop(interp);
  InterpStackPush(interp, a);
}

void PrimFun_swap(VInterp* interp)
{
  VItem* a = InterpStackAt(interp, 0);
  VItem* b = InterpStackAt(interp, 1);
  VItem temp = *a;
  *a = *b;
  *b = temp;
}

void PrimFun_over(VInterp* interp)
{
  InterpStackPush(interp, InterpStackAt(interp, 1));
}

void PrimFun_local_setA(VInterp* interp) // [A]
{
  InterpSetLocalVar(interp, 0, InterpStackPop(interp));
}

void PrimFun_local_setAB(VInterp* interp) // [AB]
{
  InterpSetLocalVar(interp, 1, InterpStackPop(interp));
  InterpSetLocalVar(interp, 0, InterpStackPop(interp));
}

void PrimFun_local_setABC(VInterp* interp) // [ABC]
{
  InterpSetLocalVar(interp, 2, InterpStackPop(interp));
  InterpSetLocalVar(interp, 1, InterpStackPop(interp));
  InterpSetLocalVar(interp, 0, InterpStackPop(interp));
}

void PrimFun_local_setABCD(VInterp* interp) // [ABCD]
{
  InterpSetLocalVar(interp, 3, InterpStackPop(interp));
  InterpSetLocalVar(interp, 2, InterpStackPop(interp));
  InterpSetLocalVar(interp, 1, InterpStackPop(interp));
  InterpSetLocalVar(interp, 0, InterpStackPop(interp));
}

void PrimFun_local_getA(VInterp* interp) // A
{
  InterpStackPush(interp, InterpGetLocalVar(interp, 0));
}

void PrimFun_local_getB(VInterp* interp) // B
{
  InterpStackPush(interp, InterpGetLocalVar(interp, 1));
}

void PrimFun_local_getC(VInterp* interp) // C
{
  InterpStackPush(interp, InterpGetLocalVar(interp, 2));
}

void PrimFun_local_getD(VInterp* interp) // D
{
  InterpStackPush(interp, InterpGetLocalVar(interp, 3));
}

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
  if (!IsList(list)) GURU_MEDITATION(FIRST_OBJECT_IS_NOT_A_LIST);

  // Get first item
  VItem* item = GetFirst(list, interp);

  // Leave empty list on the stack
  // () first => ()
  if (NULL == item) goto Exit;

  // Copy first item to data stack
  *list = *item;
  
Exit:;
}

void PrimFun_rest(VInterp* interp)
{
  VItem* list = InterpStackTop(interp);

  // Must be a list type
  if (!IsList(list)) GURU_MEDITATION(REST_OBJECT_IS_NOT_A_LIST);

  // Get first item
  VItem* item = GetFirst(list, interp);

  // Leave empty list on the stack
  // () rest => ()
  if (NULL == item) goto Exit;

  // Get second item in the list
  item = GetNext(item, interp);

  // If empty tail, leave empty list on the stack
  // (1) rest => ()
  if (NULL == item)
  {
    list->first = 0;
    goto Exit;
  }

  // Set second item as first element of the list
  SetFirst(list, item, interp);

Exit:;
}

void PrimFun_cons(VInterp* interp)
{
  // Get list and item to cons
  VItem* list = InterpStackPop(interp);
  VItem* item = InterpStackTop(interp);

  // Must be a list type
  if (!IsList(list)) GURU_MEDITATION(CONS_OBJECT_IS_NOT_A_LIST);

  // This will be the new head of the cons
  VItem newList;
  ItemInit(&newList);
  ItemSetType(&newList, ItemGetType(list));

  // Allocate new element
  VItem* newFirst = AllocItem(interp);
  if (NULL == newFirst) GURU_MEDITATION(CONS_OUT_OF_MEMORY);

  // Copy item to new element
  *newFirst = *item;

  // Get first element of the list 
  VItem* first = GetFirst(list, interp);

  if (NULL == first)
  {
    // If empty list, the new item is the last and only element
    SetFirst(newFirst, NULL, interp);
  }
  else
  {
    // Link new item to the first element of the list
    SetFirst(newFirst, first, interp);
  }

  // Set first of list to refer to the new element
  SetFirst(&newList, newFirst, interp);

  // Copy new list item to data stack
  *item = newList;
}

// Leaves list on the stack
// list item setfirst --> list
void PrimFun_setfirst(VInterp* interp)
{
  VItem* item = InterpStackPop(interp);
  VItem* list = InterpStackTop(interp);

  // Must be a list type
  if (!IsList(list)) GURU_MEDITATION(SETFIRST_OBJECT_IS_NOT_A_LIST);

  // Get first item
  VItem* first = GetFirst(list, interp);

  // Set first of empty list
  if (NULL == first)
  {
    first = AllocItem(interp);
    if (NULL == first) GURU_MEDITATION(SETFIRST_OUT_OF_MEMORY);
    SetFirst(list, first, interp);
  }

  // Preserve address of next item
  VAddr next = ItemGetNext(first);

  // Copy item
  *first = *item;

  // Restore address of next
  ItemSetNext(first, next);
}

void PrimFun_gc(VInterp* interp)
{
  InterpGC(interp);
}

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
}

// millis sleep -->
void PrimFun_sleep(VInterp* interp)
{
  VItem* item = InterpStackPop(interp);
  if (!IsTypeIntNum(item))
  {
    GURU_MEDITATION(SLEEP_NOT_INTNUM);
  }

  int millis = item->intNum;
  int seconds = millis / 1000;
  int micros = (millis % 1000) * 1000;

  sleep(seconds);
  usleep(micros);
}

void PrimFun_def(VInterp* interp)
{
  PrimFun_funify(interp);
  PrimFun_swap(interp);
  PrimFun_setglobal(interp);
}

void PrimFun_evalfile(VInterp* interp)
{
  PrimFun_readfile(interp);
  PrimFun_parse(interp);
  PrimFun_eval(interp);
}

void AddCorePrimFuns()
{
  PrimFunTableAdd("sayHi", PrimFun_sayHi);
  PrimFunTableAdd("print", PrimFun_print);
  PrimFunTableAdd("printstack", PrimFun_printstack);
  PrimFunTableAdd("eval", PrimFun_eval);
  PrimFunTableAdd("iftrue", PrimFun_iftrue);
  PrimFunTableAdd("iffalse", PrimFun_iffalse);
  PrimFunTableAdd("ifelse", PrimFun_ifelse);
  PrimFunTableAdd("setglobal", PrimFun_setglobal);
  PrimFunTableAdd("getglobal", PrimFun_getglobal);
  PrimFunTableAdd("funify", PrimFun_funify);
  PrimFunTableAdd("parse", PrimFun_parse);
  PrimFunTableAdd("readfile", PrimFun_readfile);
  PrimFunTableAdd("+", PrimFun_plus);
  PrimFunTableAdd("-", PrimFun_minus);
  PrimFunTableAdd("*", PrimFun_times);
  PrimFunTableAdd("/", PrimFun_div);
  PrimFunTableAdd("1+", PrimFun_1plus);
  PrimFunTableAdd("1-", PrimFun_1minus);
  PrimFunTableAdd("2+", PrimFun_2plus);
  PrimFunTableAdd("2-", PrimFun_2minus);
  PrimFunTableAdd("<", PrimFun_lessthan);
  PrimFunTableAdd(">", PrimFun_greaterthan);
  PrimFunTableAdd("eq", PrimFun_eq);
  PrimFunTableAdd("iszero", PrimFun_iszero);
  PrimFunTableAdd("not", PrimFun_not);
  PrimFunTableAdd("drop", PrimFun_drop);
  PrimFunTableAdd("[]", PrimFun_drop);
  PrimFunTableAdd("dup", PrimFun_dup);
  PrimFunTableAdd("[aa]", PrimFun_dup);
  PrimFunTableAdd("swap", PrimFun_swap);
  PrimFunTableAdd("[ba]", PrimFun_swap);
  PrimFunTableAdd("over", PrimFun_over);
  PrimFunTableAdd("[aba]", PrimFun_over);
  PrimFunTableAdd("[x]", PrimFun_local_setA);
  PrimFunTableAdd("[xy]", PrimFun_local_setAB);
  PrimFunTableAdd("[xyz]", PrimFun_local_setABC);
  PrimFunTableAdd("[xyzq]", PrimFun_local_setABCD);
  PrimFunTableAdd("x", PrimFun_local_getA);
  PrimFunTableAdd("y", PrimFun_local_getB);
  PrimFunTableAdd("z", PrimFun_local_getC);
  PrimFunTableAdd("q", PrimFun_local_getD);
  PrimFunTableAdd("first", PrimFun_first);
  PrimFunTableAdd("rest", PrimFun_rest);
  PrimFunTableAdd("cons", PrimFun_cons);
  PrimFunTableAdd("setfirst", PrimFun_setfirst);
  PrimFunTableAdd("gc", PrimFun_gc);
  PrimFunTableAdd("millis", PrimFun_millis);
  PrimFunTableAdd("sleep", PrimFun_sleep);
  PrimFunTableAdd("def", PrimFun_def);
  PrimFunTableAdd("evalfile", PrimFun_evalfile);
}

/*
// port socketcreate --> socket
void PrimFunx_socketcreate(VInterp* interp)
{
  VItem* port = InterpStackPop(interp);

  int socketId = socket(AF_INET, SOCK_STREAM, 0);
  if (socketId < 1)
  {
    GURU_MEDITATION(SOCKET_CREATE_ERROR);
  }

  int status = fcntl(socketId, F_SETFL, fcntl(socketId, F_GETFL, 0) | O_NONBLOCK);
  if (status == -1)
  {
    GURU_MEDITATION(SOCKET_CREATE_FCNTL_ERROR);
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
    GURU_MEDITATION(SOCKET_BIND_ERROR);
  }

  VItem socketItem;

  ItemSetType(&socketItem, TypeBufferPtr);
  socketItem.ptr = socket;

  InterpStackPush(interp, &socketItem);
}

// socket socketlisten --> socket
void PrimFunx_socketlisten(VInterp* interp)
{
  VItem* socket = InterpStackTop(interp);

  int status = listen(socket->intNum, 10);
  if (status < 0) 
  {
    GURU_MEDITATION(SOCKET_LISTEN_ERROR);
  }
}

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
      GURU_MEDITATION(SOCKET_ACCEPT_ERROR);
    }
  }
  else
  {
    ItemSetIntNum(&result, 1);
  }

  InterpStackPush(&result);
}

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
      GURU_MEDITATION(SOCKET_ACCEPT_ERROR);
    }
  }
  else
  {
    ItemSetIntNum(&result, 1);
  }

  InterpStackPush(&result);
}


// socket socketclose --> 
void PrimFunx_sockeclose(VInterp* interp)
{
  VItem* socket = InterpStackPop(interp);
close(client_socket);
}
*/
