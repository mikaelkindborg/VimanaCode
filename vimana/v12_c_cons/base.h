/*
File: base.h
Author: Mikael Kindborg (mikael@kindborg.com)

Basic data types and functions.
*/

// -------------------------------------------------------------
// Types - Vimana types are prefixed with V
// -------------------------------------------------------------

typedef unsigned char    VByte;
typedef long             VIntNum;
typedef double           VDecNum;
typedef uintptr_t        VAddr;
typedef uintptr_t        VType;

// -------------------------------------------------------------
// Byte pointers
// -------------------------------------------------------------

#define VBytePtr(ptr) ((VByte*)(ptr))
#define PtrOffset(ptr, offset) ((void*)(VBytePtr(ptr) + (offset)))

// -------------------------------------------------------------
// Boolean values
// -------------------------------------------------------------

#define FALSE 0
#define TRUE  1

// -------------------------------------------------------------
// Forward declarations
// -------------------------------------------------------------

typedef struct __VInterp VInterp;
