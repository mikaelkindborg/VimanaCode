/*
File: base.h
Author: Mikael Kindborg (mikael@kindborg.com)

Basic data types and functions.
*/

// -------------------------------------------------------------
// Types
// -------------------------------------------------------------

// All Vimana types are prefixed with 'V'

typedef unsigned char    VByte;
typedef unsigned int     VUInt;
//typedef unsigned long    VAddr;
typedef unsigned int     VAddr;
typedef unsigned long    VULong;
typedef          long    VIntNum;
typedef          double  VDecNum;

#define UInt(n) ((VUInt)(n))

// -------------------------------------------------------------
// Boolean values
// -------------------------------------------------------------

#define FALSE 0
#define TRUE  1

// -------------------------------------------------------------
// Byte pointers
// -------------------------------------------------------------

#define BytePtr(ptr) ((VByte*)(ptr))
#define BytePtrOffset(ptr, offset) (BytePtr(ptr) + (offset))

// -------------------------------------------------------------
// Forward declarations
// -------------------------------------------------------------

typedef struct __VInterp VInterp;
typedef void (*VPrimFunPtr) (VInterp*);
