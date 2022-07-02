/*
File: string.h
Author: Mikael Kindborg (mikael@kindborg.com)

String functions.
*/

#define StrEquals(s1, s2) (0 == strcmp(s1, s2))

// Creates new string
char* StrCopy(char* str)
{
  char* newStr = SysAlloc(strlen(str) + 1);
  strcpy(newStr, str);
  return newStr;
}

// Modifies the string
void StrToUpper(char* str)
{
  char* p = str;
  while (*p)
  {
    *p = toupper((unsigned char) *p);
    ++ p;
  }
}

// Modifies the string
void StrToLower(char* str)
{
  char* p = str;
  while (*p)
  {
    *p = tolower((unsigned char) *p);
    ++ p;
  }
}

// Check if a string begins with the given substring
int StrStartsWith(char* str, char* substr)
{
  char* pStr = str;
  char* pSubStr = substr;

  while (('\0' != *pStr) && ('\0' != *pSubStr))
  {
    if (*pStr != *pSubStr) return 0; // Not found

    ++ pStr;
    ++ pSubStr;
  }

  return 1; // Found
}
