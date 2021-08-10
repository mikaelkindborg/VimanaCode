/*
File: item.h
Author: Mikael Kindborg (mikael@kindborg.com)

Basic object with GC support.
*/

typedef struct _VObjHeader
{
  Type  type;
  Index gcMarker; // Mark flag for GC
}
VObjHeader;

typedef struct _VObj
{
  VObjHeader header;
}
VObj;
