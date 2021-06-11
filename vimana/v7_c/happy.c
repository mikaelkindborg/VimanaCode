#include <stdio.h>
int main()
{
  char c[] = { 72, 65, 80, 80, 89, 32, 66, 73, 82, 84, 72, 68, 65, 89, 10, 0 }, *p = c; while (*p) printf("%c", *p++);
  /*int i = 0;
  printf("Hello ");
  while (c[i]) { printf("%c", c[i]); i++; }
  printf(" World\n");*/
}