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
typedef unsigned int     VUInt;
typedef unsigned long    VAddr;
//typedef unsigned int     VAddr;
typedef unsigned long    VULong;
typedef          long    VIntNum;
typedef          double  VDecNum;

#define UInt(n) ((VUInt)(n))
#define BytePtr(ptr) ((VByte*)(ptr))
#define PointerOffset(ptr, offset) ((void*)(BytePtr(ptr) + (offset)))

// -------------------------------------------------------------
// Boolean values
// -------------------------------------------------------------

#define FALSE 0
#define TRUE  1

// -------------------------------------------------------------
// Forward declarations
// -------------------------------------------------------------

typedef struct __VInterp VInterp;
typedef void (*VPrimFunPtr) (VInterp*);
