/*
File: vobj.h
Author: Mikael Kindborg (mikael@kindborg.com)

Abstract "base class" for Vimana objects (lists and strings).
*/

// OBJECT HEADER -----------------------------------------------

typedef struct __VObj
{
  VType        type;         // Object type
#ifdef GC_REFCOUNT
  VIndex       refCount;     // Reference counter
#endif
}
VObj;
