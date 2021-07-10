// Experiments based on this article:
// https://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/

#include <stdlib.h>
#include <stdio.h>

typedef struct myObject 
{
  struct myObject* next;
  int value;
} 
Object;

typedef struct 
{
  Object* firstObject;
} 
VM;

VM* newVM() 
{
  VM* vm = malloc(sizeof(VM));
  vm->firstObject = NULL;
  return vm;
}

void vmNewObject(VM* vm, int value)
{
  Object* object = malloc(sizeof(Object));
  object->value = value;
  object->next = vm->firstObject;
  vm->firstObject = object;
  //printf("Adding: %d\n", object->value);
}

int vmFreeObject(VM* vm, int index)
{
  Object** object = &(vm->firstObject);
  while (*object && index > 0)
  {
    -- index;
    object = &((*object)->next);
  }
  if (*object)
  {
    printf("Removing: %d\n", (*object)->value);
    Object* obj = *object;
    *object = (*object)->next;
    free(obj);
    return 1;
  }
  else
  {
    printf("Index out of bounds\n");
    return 0;
  }
}

void vmCreateObjects(VM* vm)
{
  Object* object = vm->firstObject;
  int i = 1;
  while (i < 11)
  {
    vmNewObject(vm, i * 100);
    ++ i;
  }
}

void vmPrintObjects(VM* vm)
{
  Object* object = vm->firstObject;
  while (object)
  {
    printf("%d\n", object->value);
    object = object->next;
  }
}

int main()
{
  printf("Hi World\n");

  VM* vm = newVM();
  vmCreateObjects(vm);
  vmPrintObjects(vm);
  vmFreeObject(vm, 9);
  vmPrintObjects(vm);
  while (vmFreeObject(vm, 0))
  {
    vmPrintObjects(vm);
  }
  vmPrintObjects(vm);
  free(vm);

  return 0;
}
