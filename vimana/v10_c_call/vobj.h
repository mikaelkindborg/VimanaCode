/*
File: vobj.h
Author: Mikael Kindborg (mikael@kindborg.com)

Abstract "base class" for Vimana objects (lists and strings).
*/

// OBJECT HEADER -----------------------------------------------

typedef struct __VObj
{
  VType type; // Object type and associated gc data
}
VObj;

/*
The field "type" has the following bits:

Bit   Meaning
----------------------------------------------------------------
0     List flag
1     Function flag (a function is also a list)
2     String flag
3     GC mark (0 or 1)
*/

// OBJECT TYPES ------------------------------------------------

#define ObjTypeMask           7
#define ObjTypeList           1
#define ObjTypeFun            3 // Bit 0 and 1
#define ObjTypeString         4
#define ObjGCMark             8

#define ObjIsList(obj)        ( (ObjType(obj) & ObjTypeList)   == ObjTypeList )
#define ObjIsString(obj)      ( (ObjType(obj) & ObjTypeString) == ObjTypeString )
#define ObjIsFun(obj)         ( (ObjType(obj) & ObjTypeFun)    == ObjTypeFun )

#define AsObj(obj)            ( (VObj*)(obj) )
#define ObjType(obj)          ( (AsObj(obj)->type) & ObjTypeMask )
#define ObjSetType(obj, type) ( ObjSetTypeImpl(AsObj(obj), type) )

#define ObjIsMarked(obj)      ( (AsObj(obj)->type) & ObjGCMark )
#define ObjSetMark(obj)       ( ObjSetMarkImpl(AsObj(obj)) )
#define ObjClearMark(obj)     ( ObjClearMarkImpl(AsObj(obj)) )

static inline void ObjSetTypeImpl(VObj* obj, VType type)
{
  // Clear ObjTypeMask and set type
  obj->type = (obj->type  & ~ObjTypeMask) | type;
}

static inline void ObjSetMarkImpl(VObj* obj)
{
  obj->type = obj->type | ObjGCMark;
}

static inline void ObjClearMarkImpl(VObj* obj)
{
  obj->type = obj->type & ~ObjGCMark;
}
