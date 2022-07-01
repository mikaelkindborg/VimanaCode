/*
File: base.h
Author: Mikael Kindborg (mikael@kindborg.com)

Basic data types and functions.
*/

// -------------------------------------------------------------
// Types
// -------------------------------------------------------------

typedef unsigned char    VByte;
typedef unsigned long    VAddr;
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
#define BytePtrOffset(ptr, offset) (BytePtr(ptr) + (offset))

// -------------------------------------------------------------
// Forward declarations
// -------------------------------------------------------------

typedef struct __VInterp VInterp;
typedef void (*VPrimFunPtr) (VInterp*);
