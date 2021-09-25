/*
File: vobj.h
Author: Mikael Kindborg (mikael@kindborg.com)

Abstract "base class" for Vimana objects (lists and strings).
*/

// OBJECT TYPES ------------------------------------------------

#define ObjTypeList   0
#define ObjTypeString 1
#define ObjTypeFun    2

#define ObjIsList(obj)   ( (ObjType(obj) & 1) == ObjTypeList )
#define ObjIsString(obj) ( (ObjType(obj) & 1) == ObjTypeString )
#define ObjIsFun(obj)    ( (ObjType(obj) & 2) == ObjTypeFun )

// OBJECT HEADER -----------------------------------------------

typedef struct __VObj
{
  VType type; // Object type and associated gc data
}
VObj;

// TODO:
// Put mark in the high bit
// Use type also for refcount:
// - Type 2 bits: 1 bit List/String, 1 bit fun flag
// - Counter 6 bits

/*
Bit  Meaning
----------------------------------------------------------------
0     List/String flag
1     Function flag
2..7  Sweep mark (0 or 1) or ref count (6 bits on Arduino) 
*/


#define ObjCast(obj)          ((VObj*)(obj))
#define ObjType(obj)          ((ObjCast(obj)->type) & 3)
#define ObjSetType(obj, type) (ObjSetTypeImpl(ObjCast(obj), type))

static inline void ObjSetTypeImpl(VObj* obj, VType type)
{
  obj->type = (obj->type & ~3) | (type & 3);
}

#ifdef GC_MARKSWEEP

  #define ObjIsMarked(obj)  ((ObjCast(obj)->type) & 4)
  #define ObjSetMark(obj)   (ObjSetMarkImpl(ObjCast(obj)))
  #define ObjClearMark(obj) (ObjClearMarkImpl(ObjCast(obj)))

  static inline void ObjSetMarkImpl(VObj* obj)
  {
    obj->type = obj->type | 4; // Set bit 3
  }

  static inline void ObjClearMarkImpl(VObj* obj)
  {
    obj->type = obj->type & 3; // Clear bit 3 an above
  }

#endif

#ifdef GC_REFCOUNT

  #define ObjGetRefCount(obj) ((ObjCast(obj)->type) >> 2)

  static inline void ObjSetRefCount(VObj* obj, VIndex count)
  {
    obj->type = obj->type | (count << 2);
  }

  static inline void ObjIncrRefCount(VObj* obj)
  {
    ObjSetRefCount(obj, ObjGetRefCount(obj) + 1);
  }

  static inline void ObjDecrRefCount(VObj* obj)
  {
    ObjSetRefCount(obj, ObjGetRefCount(obj) - 1);
  }

#endif
