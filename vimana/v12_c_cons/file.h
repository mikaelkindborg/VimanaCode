/*
File: file.h
Author: Mikael Kindborg (mikael@kindborg.com)
*/

typedef FILE VPrintStream;

#define FILE_STAT_MODIFIED 1
#define FILE_STAT_NOT_MODIFIED 2
#define FILE_STAT_ERROR -1

char* FileRead(char* fileName)
{
  char*  buffer;
  size_t size;
  int    c;

  VPrintStream* stream = open_memstream(&buffer, &size);
  FILE* file = fopen(fileName, "r");

  if (file) 
  {
    while ((c = getc(file)) != EOF)
    {
      fputc(c, stream);
    }

    fputc(0, stream);
    fclose(file);
    fclose(stream);

    // Increment SysAllocCounter
    SysAllocCounterIncr();

    return buffer;
  }
  else
    return NULL;
}

// time_t lastUpdate = 0;
int FileIsModified(char* file, time_t* lastUpdate)
{
  struct stat statbuf;
  
  int result = stat(file, &statbuf);
  if (result) return FILE_STAT_ERROR;
  
  int status = FILE_STAT_NOT_MODIFIED;

  if ((0 != *lastUpdate) && (statbuf.st_mtime > *lastUpdate))
  {
    status = FILE_STAT_MODIFIED;
  }

  *lastUpdate = statbuf.st_mtime;

  return status;
}
