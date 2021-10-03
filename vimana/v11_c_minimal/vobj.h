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
0     List/String flag
1     Function flag
2..7  Ref count (6 bits on Arduino)
*/

// OBJECT TYPES ------------------------------------------------

#define ObjTypeList            0
#define ObjTypeString          1
#define ObjTypeMask            1
#define ObjTypeFun             2
#define ObjTypeFunMask         2

#define ObjRefCountTypeMask    3
#define ObjRefCountShift       2

#define AsObj(obj)             ( (VObj*)(obj) )
#define ObjType(obj)           ( AsObj(obj)->type )
#define ObjSetType(obj, type)  ( ObjSetTypeImpl(AsObj(obj), type) )

#define ObjIsList(obj)         ( (ObjType(obj) & ObjTypeMask) == ObjTypeList )
#define ObjIsString(obj)       ( (ObjType(obj) & ObjTypeMask) == ObjTypeString )

#define ObjIsFun(obj)          ( (ObjType(obj) & ObjTypeFunMask) == ObjTypeFun )
#define ObjSetFunFlag(obj)     ( ObjSetFunFlagImpl(AsObj(obj), ObjTypeFun) )
#define ObjClearFunFlag(obj)   ( ObjSetFunFlagImpl(AsObj(obj), 0) )

static inline void ObjSetTypeImpl(VObj* obj, VType value)
{
  obj->type = (obj->type & ~ObjTypeMask) | value;
}

static inline void ObjSetFunFlagImpl(VObj* obj, VType value)
{
  obj->type = (obj->type & ~ObjTypeFunMask) | value;
}

#define ObjGetRefCount(obj) ((AsObj(obj)->type) >> ObjRefCountShift)

static inline void ObjSetRefCount(VObj* obj, VIndex count)
{
  obj->type = (obj->type & ObjRefCountTypeMask) | (count << ObjRefCountShift);
}

static inline void ObjIncrRefCount(VObj* obj)
{
  ObjSetRefCount(obj, ObjGetRefCount(obj) + 1);
}

static inline void ObjDecrRefCount(VObj* obj)
{
  ObjSetRefCount(obj, ObjGetRefCount(obj) - 1);
}
