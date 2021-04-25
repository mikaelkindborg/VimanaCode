#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//typedef char[32] Symbol;
//typedef Symbol[];

typedef struct MyList List;
typedef struct MyItem Item;

typedef struct MyItem
{
  int type;
  union
  {
    char symbol[32];
    double number;
    List* list;
  } 
  data;
}
Item;

typedef struct MyList
{
  int length;
  int maxLength;
  Item* items;
}
List;

List* Interp_ListCreate(int size)
{
  List* list = malloc(sizeof(List));
  Item* items = malloc(size * sizeof(Item));
  list->items = items;
  list->length = 0;
  list->maxLength = size;
  return list;
}

void Interp_ListFree(List* list)
{
  free(list->items);
  free(list);
}

void Interp_ListPush(List* list, Item item)
{
  list->items[list->length] = item;
  list->length++;
}

Item Interp_ListGet(List* list, int index)
{
  return list->items[index];
}


void Interp_ListPrint(List* list)
{
  printf("(");
  
  for (int i = 0; i < list->length; i++)
  {
    Item item = Interp_ListGet(list, i);
    if (item.type == 1)
    {
      printf("%s", item.data.symbol);
    }
    else if (item.type == 2)
    {
      printf("%f", item.data.number);
    }
    else if (item.type == 3)
    {
      Interp_ListPrint(item.data.list);
    }
    
    if (i < list->length - 1)
    {
      printf(" ");
    }
  }
  printf(")");
}
  
Item Interp_ItemWithSymbol(char* symbol)
{
  Item item;
  item.type = 1;
  strcpy(item.data.symbol, symbol);
  return item;
}

Item Interp_ItemWithList(List* list)
{
  Item item;
  item.type = 3;
  item.data.list = list;
  return item;
}

int Interp_ParserWorker(char* code, int i, int length, List* list);

List* Interp_Parse(char* code)
{
  List* list = Interp_ListCreate(20);
  Interp_ParserWorker(code, 0, strlen(code), list);
  return list;
}

int Interp_ParserWorker(char* code, int i, int length, List* list)
{
  int copying = 0;
  char token[32];
  char* ptoken;

  while (i < length)
  {
    // Begin list.
    if (code[i] == '(')
    {
      //printf("BEGIN LIST\n");
      List* childlist = Interp_ListCreate(20);
      Item item = Interp_ItemWithList(childlist);
      Interp_ListPush(list, item);
      i = Interp_ParserWorker(code, i + 1, length, childlist);
      continue;
    }
    
    // Skip whitespace or end list.
    if (code[i] == ' '  || code[i] == '\t' || 
        code[i] == '\n' || code[i] == '\r' ||
        code[i] == ')')
    {
      // Copy end.
      if (copying)
      {
        copying = 0;
       *ptoken = 0;
        //printf("%s\n", token);
        Item item = Interp_ItemWithSymbol(token);
        Interp_ListPush(list, item);
        
        // End list.
        if (code[i] == ')')
        {
          //printf("END LIST\n");  
          return i;
        }
      }
    }
    else
    {
      // If we are not copying begin.
      if (!copying)
      {
        copying = 1;
        ptoken = token;
      }
      
      // copy to token
     *ptoken = code[i];
      ptoken++;
    }
    
    i++;
  }
  
  if (copying)
  {
    // end token
    *ptoken = 0;
    //printf("%s\n", token);
    Item item = Interp_ItemWithSymbol(token);
    Interp_ListPush(list, item);
  }
      
  return i;
}


/*

    //printf("%c", *p);
  
  //char* tokens = malloc(sizeof(Symbol) * 1000);
  char* p = code;
  //char* t = tokens;

  char token[32];
  char* ptoken;
  char* p1 = NULL;
  
int interp_create_list(&$tokens)
{
  $list = [];
  
  while (TRUE):
    if (count($tokens) === 0):
      return $list;
    endif;
    
    $next = array_shift($tokens);
    
    if ($next === ")"):
      return $list;
    endif;
    
    if ($next === "("):
      $next = interp_create_list($tokens);
    endif;
    
    // Convert string to number.
    if (is_numeric($next)):
      $next = $next + 0;
    endif;
    
    array_push($list, $next);
  endwhile;
}
*/

void play_with_list()
{
  List* list = Interp_ListCreate(10);
  
  Item item;
  
  strcpy(item.data.symbol, "Hello");
  item.type = 1;
  Interp_ListPush(list, item);
  
  item.data.number = 33.3;
  item.type = 2;
  Interp_ListPush(list, item);
  
  printf("Size of list: %i\n", list->length);
  
  item = Interp_ListGet(list, 0);
  printf("%s\n", item.data.symbol);
  
  item = Interp_ListGet(list, 1);
  printf("%f\n", item.data.number);
}

int main()
{
  printf("Hello World\n");
  
  List* list = Interp_Parse("HELLO PRINTLN (1 2 +) DO PRINTLN");
  Interp_ListPrint(list);
  printf("\n");
  /*
  for (int i = 0; i < list->length; i++)
  {
    Item item = Interp_ListGet(list, i);
    if (item.type == 1)
      printf("SYMBOL: %s\n", item.data.symbol);
  }
  */
  
  //play_with_list();
}
