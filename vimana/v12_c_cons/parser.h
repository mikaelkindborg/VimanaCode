/*
File: parser.h
Author: Mikael Kindborg (mikael@kindborg.com)

Parser for source code.
*/

#define IsWhiteSpace(c) \
  (' ' == (c) || '\t' == (c) || '\n' == (c) || '\r' == (c))
#define IsLeftParen(c) ('(' == (c))
#define IsRightParen(c) (')' == (c))
#define IsParen(c) (IsLeftParen(c) || IsRightParen(c))

#define IsStringSeparator(c) ('\'' == (c))
#define IsEndOfString(c) ('\0' == (c))

#define IsWhiteSpaceOrSeparatorOrEndOfString(c) \
  (IsWhiteSpace(c) || IsParen(c) || IsStringSeparator(c) || IsEndOfString(c))

int ParseToken(char* p, char** next)
{
  char buf[512];
  char* pBuf = buf;

  while (!IsWhiteSpaceOrSeparatorOrEndOfString(*p))
  {
    *pBuf = *p;
    ++ pBuf;
    ++ p;
  }

  *next = p;

  printf("C:%c\n", (char)buf[0]);

  return (int) buf[0];
}

char* ParseString(char* p, char** next)
{
  int   length = 0;
  int   bufSize = 1024;
  char* buf = SysAlloc(bufSize);
  char* pBuf = buf;

  while (! (IsStringSeparator(*p) || IsEndOfString(*p)) )
  {
    *pBuf = *p;
    ++ pBuf;
    ++ p;
    ++ length;
  }

  *next = p + 1;

  *pBuf = 0;
  buf = realloc(buf, length + 1);

  return buf;
}

// Returns a list item
VItem* ParseCode(char* code, char** next, VMem* mem)
{
  VItem* first = NULL;
  VItem* item = NULL;
  VItem* prev;
  VItem* list;
  
  char* p = code;

  while (!IsEndOfString(*p))
  {
    if (IsWhiteSpace(*p))
    {
      // Skip whitespace
      ++ p;
    }
    else
    if (IsLeftParen(*p))
    {
      printf("parse child\n");
      // Parse child list
      item = ParseCode(p + 1, &p, mem);
    }
    else
    if (IsRightParen(*p))
    {
      // Done parsing list
      ++ p;
      *next = p;
      goto Exit;
    }
    else
    if (IsStringSeparator(*p))
    {
      printf("parse string\n");
      char* string = ParseString(p + 1, &p);
      printf("string: %s\n", string);
      item = MemAllocItem(mem);
      ItemSetString(item, string);
    }
    else
    {
      printf("parse token\n");
      // Get next token
      int num = ParseToken(p, &p);
      item = MemAllocItem(mem);
      ItemSetIntNum(item, num);
    }

    // Add item to list
    if (NULL != item)
    {
      if (NULL == first)
        first = item;
      else
        MemItemSetNext(mem, prev, item);

      prev = item;
      item = NULL;
    }
  }

Exit:
  list = MemAllocItem(mem);
  MemItemSetFirst(mem, list, first);
  return list;
}

VItem* ParseSourceCode(char* sourceCode, VMem* mem)
{
  char* p;
  return ParseCode(sourceCode, &p, mem);
}
