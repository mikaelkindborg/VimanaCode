
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/******************************************************

Compile with:

  cc monitor.c -o monitor

Run with:

  ./monitor hello.c
  
******************************************************/

int main(int argc, char *argv[])
{
  printf("Hello Live Compile\n");
  
  if (argc != 2) 
  {
     printf("Kindly specify the file to monitor %s\n", argv[1]);
  }
  
  char* file = argv[1];
  
  printf("Monitoring: %s\n", file);
     
  struct stat statbuf;
  time_t lastUpdate = 0;
  int res;
  char command[128];
  char outfile[32];
  
  strcpy(outfile, file);
  outfile[strlen(file) - 2] = 0;
  
  while (1)
  {
    res = stat(file, &statbuf);
    if (res)
    {
      printf("stat returned an error\n");
      return 0;
    }
    
    if (lastUpdate == 0)
    {
      lastUpdate = statbuf.st_mtime;
    }
    else if (statbuf.st_mtime > lastUpdate)
    {
      printf("Recompile!\n");
      
      strcpy(command, "cc ");
      strcat(command, file);
      strcat(command, " -o ");
      strcat(command, outfile);
      strcat(command, "; ./");
      strcat(command, outfile);
      printf("%s\n", command);
      
      res = system(command);
      
      lastUpdate = statbuf.st_mtime;
    }
    
    //printf("Last modification: %li\n", statbuf.st_mtime);
    
    sleep(3);
  }
  
  return 0;
}
