/*
File: symbolmemory.h
Author: Mikael Kindborg (mikael@kindborg.com)

Compact way to allocate immutable symbols within a block 
of memory (no need for malloc of individual symbols).

Memory layout:

Symbols are allocated in sequence.
0 char is the string terminator.

Example with three symbol entries (0 is char zero '\0'):

  One0Two0Tree0
*/

// -------------------------------------------------------------
// Symbol memory struct
// -------------------------------------------------------------

typedef struct __VSymbolMemory
{
  char*  start;    // Start of string memory
  char*  nextFree; // Points to next free block in string memory
  char*  pos;      // Current position to write to
  int    size;     // Number of chars that can be stored in string memory
}
VSymbolMemory;

// -------------------------------------------------------------
// Global instance
// -------------------------------------------------------------

// Global symbol memory instance
static VSymbolMemory* GlobalSymbolMemory;  

// Get global instance
VSymbolMemory* SymbolMemGlobal()
{
  return GlobalSymbolMemory;
}

void SymbolMemInit(VSymbolMemory* mem, int numChars);

// Initialize global instance
void SymbolMemInitGlobal(void* mem, int numChars)
{
  GlobalSymbolMemory = mem;
  SymbolMemInit(mem, numChars);
}

// -------------------------------------------------------------
// Symbol memory functions
// -------------------------------------------------------------

// Return number of bytes needed to hold header struct plus array
int SymbolMemByteSize(int numChars)
{
  return sizeof(VSymbolMemory) + (sizeof(char) * numChars);
}

void SymbolMemInit(VSymbolMemory* mem, int numChars)
{
  mem->start = (char*) PtrOffset(mem, sizeof(VSymbolMemory));
  mem->nextFree = mem->start;
  mem->pos = mem->nextFree;
  mem->size = numChars;
}

// Return pointer to next free block
char* SymbolMemGetNextFree(VSymbolMemory* mem)
{
  return mem->nextFree;
}

// Finish writing to the current block
void SymbolMemWriteFinish(VSymbolMemory* mem)
{
  ++ mem->pos;
  mem->nextFree = mem->pos;
}

// Reset the write position to beginning of free block
void SymbolMemResetPos(VSymbolMemory* mem)
{
  mem->pos = mem->nextFree;
}

// Write a character
void SymbolMemWriteChar(VSymbolMemory* mem, char c)
{
  if (mem->pos + 2 >= mem->start + mem->size)
  {
    GURU_MEDITATION(STRING_MEMORY_OUT_OF_SPACE);
  }

  *(mem->pos) = c;
  ++ mem->pos;
  *(mem->pos) = 0;
}
