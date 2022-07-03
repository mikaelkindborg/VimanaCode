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
typedef unsigned int     VType;
typedef unsigned int     VAddr;
typedef          long    VIntNum;
typedef          double  VDecNum;

// -------------------------------------------------------------
// Boolean values
// -------------------------------------------------------------

#define FALSE 0
#define TRUE  1

// -------------------------------------------------------------
// Byte pointers
// -------------------------------------------------------------

#define BytePtr(ptr) ((VByte*)(ptr))
#define PtrOffset(ptr, offset) ((void*)(BytePtr(ptr) + (offset)))

// -------------------------------------------------------------
// Forward declarations
// -------------------------------------------------------------

typedef struct __VInterp VInterp;
typedef void (*VPrimFunPtr) (VInterp*);
VPrimFunPtr LookupPrimFunPtr(int index);
