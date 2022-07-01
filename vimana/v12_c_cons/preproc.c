#include <stdio.h>

#define STRINGIFY(x) #x
#define STRVALUE(x) STRINGIFY(x)

//#define CODE_A 1

enum GURU_TABLE
{
  CODE_A = 1,
  CODE_B,
  CODE_C
};

#define GURU_MEDITATION(name) GuruMeditation(name, #name)

//STRINGIFY(CODE_A)
//STRVALUE(CODE_A)

void GuruMeditation(int id, char* name)
{
  printf("GURU: %i %s\n", id, name);
}

int main()
{
  GURU_MEDITATION(CODE_A);
  GURU_MEDITATION(CODE_C);
}
