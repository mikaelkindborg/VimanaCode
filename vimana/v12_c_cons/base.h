/*
File: base.h
Author: Mikael Kindborg (mikael@kindborg.com)

Basic data types and functions.
*/

// -------------------------------------------------------------
// Types
// -------------------------------------------------------------

// Vimana types are prefixed with 'V'

typedef unsigned char    VByte;
typedef long             VIntNum;
typedef double           VDecNum;

#define VIMANA_64

#ifdef VIMANA_64
typedef uintptr_t        VAddr;
typedef uintptr_t        VType;
#else
typedef unsigned int     VAddr;
typedef unsigned int     VType;
#endif

// -------------------------------------------------------------
// Boolean values
// -------------------------------------------------------------

#define FALSE 0
#define TRUE  1

// -------------------------------------------------------------
// Byte pointers
// -------------------------------------------------------------

#define VBytePtr(ptr) ((VByte*)(ptr))
#define PtrOffset(ptr, offset) ((void*)(VBytePtr(ptr) + (offset)))

// -------------------------------------------------------------
// Forward declarations
// -------------------------------------------------------------

typedef struct __VInterp VInterp;
