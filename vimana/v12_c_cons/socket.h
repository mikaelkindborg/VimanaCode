/*
File: socket.h
Author: Mikael Kindborg (mikael@kindborg.com)
*/

typedef struct __VSocket
{
  int                socketId;
  struct sockaddr_in address;
}
VSocket;
