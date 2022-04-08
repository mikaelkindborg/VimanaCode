/*
File: string.h
Author: Mikael Kindborg (mikael@kindborg.com)

Basic string functions.
*/

#define StrEquals(s1, s2) (0 == strcmp(s1, s2))

char* StrCopy(char* str)
{
  char* newStr = SysAlloc(strlen(str + 1));
  strcpy(newStr, str);
  return newStr;
}

#define StrFree(str) SysFree(str)

void StrToUpper(char* s)
{
  char* p = s;
  while (*p)
  {
    *p = toupper((unsigned char) *p);
    ++ p;
  }
}

void StrToLower(char* s)
{
  char* p = s;
  while (*p)
  {
    *p = tolower((unsigned char) *p);
    ++ p;
  }
}
