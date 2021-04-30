#include <stdio.h>
#include <stdlib.h>

// cc -E macrotest.c

#define PrintStr(str, args...) printf(str "\n", ## args)
#define PrintDebug(str, args...) printf("[DEBUG] " str "\n", ## args)
#define ErrorExit(str, args...) printf("[ERROR] " str "\n", ## args); exit(0)

// Test it

int main()
{
  PrintStr("Hello %s %i", "World", 42);
  PrintDebug("Hello World");
  ErrorExit("Holy Crap");
  PrintDebug("Unseen");
}

