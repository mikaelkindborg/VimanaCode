#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//typedef char[32] Symbol;
//typedef Symbol[];



    
/*#define List_Push(L, X) _Generic((X), \
    char*: List_PushString, \
    long: List_PushLong, \
    )(L, X)
    
    #define Serial_print(X) _Generic((X), \
    char*: Serial_print_s, \
    signed long: Serial_print_i, \
    signed int: Serial_print_i, \
    signed char: Serial_print_c, \
    unsigned long: Serial_print_u, \
    unsigned int: Serial_print_u, \
    unsigned char: Serial_print_u \
    )(X)
    
printf("%d\n", _Generic(1L, float:1, double:2, 
                            long double:3, default:0));
  */
  
  
void List_PushString(int L, char* S)
{
  printf("I am pushing a string: %s\n", S);
}


void List_PushLong(int L, long N)
{
  printf("I am pushing a long: %li\n", N);
}

void foobar(char* s, char* s2)
{
  printf("I am pushing a long: %s\n", s);
}


void foobar_int(char* s, int i)
{
  printf("I am pushing a long: %s %i\n", s, i);
}



#define List_Push(L, X) _Generic((X), char*: List_PushString, long: List_PushLong)(L, X)
    
//#define FOO(X) _Generic((X), char*: printf, int: printf)(X)
#define FOO(X, Y) _Generic((Y), char*: foobar, \
int: foobar_int)(X, Y)


int main()
{
  printf("Hello World\n");
  
  //char* s =_Generic("a", char*: "Hello Char*", int: "Hello Int");
  //printf("s = %s\n", s);
  
  //_Generic("HEJ", char*: printf("Hello Char*"), int: printf("Hello Int"));
  
  foobar("HEJ", "HOPP");
  FOO("HEJ2", 42);
  
  List_PushLong(2, 77);
  List_PushString(2, "H3");
  
  
  List_Push(2, "H4");

/*
  char* s = "HELLO";
  int i = 42;
  List_Push(s);
  List_Push(i);
*/
}
