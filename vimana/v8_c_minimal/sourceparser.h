/*
File: sourceparser.h
Author: Mikael Kindborg (mikael@kindborg.com)

Parser for source code. Outputs symbolic code as a string.
*/

typedef FILE VPrintStream;

//#define PrintToStream(stream, str, args...) fprintf(stream, str, ## args)

// This version checks for integers
VBool TokenIsNumber(char* token)
{
  char* p = token;

  // Handle the minus sign.
  if ('-' == *p)
  {
    if (1 == strlen(token))
      return FALSE;
    else
      ++ p;
  }

  while ('\0' != *p)
  {
    if (!isdigit(*p))
      return FALSE;
  }

  return TRUE;
}

// Token returned in pBuf must be deallocated.
char* ParseToken(char* p, char** pBuf)
{
  int bufSize = 1000;
  *pBuf = MemAlloc(bufSize);
  char* pCurrent = *pBuf;
  int size = 0;
  while (!IsWhiteSpaceOrSeparator(*p)) 
  {
    // TODO: Check buffer overflow and realloc buffer
    if (size > bufSize - 1) GuruMeditaton(PARSETOKEN_BUFFER_OVERFLOW);
    *pCurrent = *p;
    ++ p;
    ++ pCurrent;
    ++ size;
  }
  *pCurrent = '\0';
  *pBuf = realloc(*pBuf, strlen(*pBuf) + 1);
  return p;
}

char* SourceParserCopyString(char* p, VPrintStream* stream)
{
  fputc('\'', stream);
  while (!IsQuote(*p)) 
  {
    fputc(*p, stream);
    ++ p;
  }
  fputc('\'', stream);
  return p + 1;
}

// Convert a source code string to symbolic code.
void SourceParserWorker(char* p, VPrintStream* stream, VSymbolDict* dict)
{
  char   c;
  char*  token;
  VIndex id;
  
  while ('\0' != *p)
  {
    if (IsWhiteSpace(*p))
    {
      ++ p;
    }
    if (IsParen(*p))
    {
      fputc(*p, stream);
      ++ p;
    }
    else
    if (IsQuote(*p))
    {
      p = SourceParserCopyString(p + 1, stream);
    }
    else // Parse token
    {
      // Get next token in string.
      p = ParseToken(p, &token);
      if (TokenIsNumber(token))
      {
        // Token is number.
        fprintf(stream, "N%s", token);
      }
      else
      {
        // Token is primfun or symbol.
        id = SymbolDictLookupPrimFunName(dict, token);
        if (id > -1)
        {
          // Print primfun id to stream.
          fprintf(stream, "P%d", id);
        }
        else
        {
          id = SymbolDictLookupSymbolName(dict, token);
          if (id < 0)
          {
            // Add symbol to dict.
            id = SymbolDictAddSymbolName(dict, token);
          }
          
          // Print symbol id to stream.
          fprintf(stream, "S%d", id);
        }
      }
      MemFree(token);
    }
  }
}

// Returned string must be deallocated.
char* GenerateSymbolicCode(char* sourceCode, VSymbolDict* dict)
{
  char* buffer;
  size_t size;
  VPrintStream* stream = open_memstream(&buffer, &size);
  SourceParserWorker(sourceCode, stream, dict);
  fclose(stream);
  puts(buffer);
  return buffer;
}

VList* ParseSourceCode(char* sourceCode, VSymbolDict* dict)
{
  char* symbolicCode = GenerateSymbolicCode(sourceCode, dict);
  VList* codeList = ParseSymbolicCode(symbolicCode);
  free(symbolicCode);
  return codeList;
}
