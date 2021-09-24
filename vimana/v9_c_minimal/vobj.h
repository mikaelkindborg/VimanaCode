/*
File: vobj.h
Author: Mikael Kindborg (mikael@kindborg.com)

Abstract "base class" for Vimana objects (lists and strings).
*/

// OBJECT TYPES ------------------------------------------------

#define ObjTypeList          1 // List with items as children
#define ObjTypeFun           2 // List with items as children
#define ObjTypeString        3 // Has no items as children

#define ObjHasChildren(list) \
  ( (ObjType(list) == ObjTypeList) || (ObjType(list) == ObjTypeFun) )

// OBJECT HEADER -----------------------------------------------

typedef struct __VObj
{
  VType        type;         // Object type
#ifdef GC_REFCOUNT
  VIndex       refCount;     // Reference counter
#endif
}
VObj;

#ifdef GC_MARKSWEEP

#define ObjCast(obj) ((VObj*)(obj))
#define ObjType(obj) ( (ObjCast(obj)->type) & 127 )
#define ObjSetType(obj, type) (ObjSetTypeImpl(ObjCast(obj), type))
#define ObjIsMarked(obj) ( ((ObjCast(obj)->type) & 128) == 128 )
#define ObjSetMark(obj) (ObjSetMarkImpl(ObjCast(obj)))
#define ObjClearMark(obj) (ObjClearMarkImpl(ObjCast(obj)))

void ObjSetTypeImpl(VObj* obj, VType type)
{
  obj->type = (obj->type & 128) | (type & 127);
}

void ObjSetMarkImpl(VObj* obj)
{
  obj->type = obj->type | 255;
}

void ObjClearMarkImpl(VObj* obj)
{
  obj->type = obj->type & 127;
}

#endif
