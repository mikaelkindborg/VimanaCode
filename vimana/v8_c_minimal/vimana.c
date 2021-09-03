#include "vimana.h"

// MAIN ------------------------------------------------

// TODO: Make file with item tests.

int main(int numargs, char* args[])
{
  long x = -255;
  PrintBinaryULong(15);
  PrintBinaryULong(x);
  PrintBinaryULong(x << 3);
  VmItem item = ItemWithNumber(x);
  PrintBinaryULong(item.value.bits);
  VmNumber num = ItemNumber(item);
  PrintBinaryULong(num);
  printf("Number: %ld\n", num);

  unsigned long xx = 0;
  xx = ~xx;
  PrintBinaryULong(xx);
  xx = xx >> 1; // Generates error below
  xx = xx >> 4; // Passes below
  PrintBinaryULong(xx);
  printf("xx: %lu\n", xx);
  printf("xx: %ld\n", (long)xx);
  VmItem errorTest = ItemWithNumber(xx);

  PrintMemStat();
}
