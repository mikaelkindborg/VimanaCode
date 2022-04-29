#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

void mysend(int socket, char* data)
{
  send(socket, data, strlen(data), 0);
}

int main()
{
  int status;
 
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 1)
  {
    printf("Error creating server socket\n");
    goto Exit;
  }

  status = fcntl(server_socket, F_SETFL, 
    fcntl(server_socket, F_GETFL, 0) | O_NONBLOCK);
  if (status == -1)
  {
    printf("Error calling fcntl\n");
    goto Exit;
  }

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(8888);

  status = bind(server_socket, (struct sockaddr *) &address, sizeof(address));
  if (status < 0)
  {
    printf("Error bind: %i\n", status);
    goto Exit;
  }

  int bufsize = 4000;
  char *buffer = malloc(bufsize);

  while (1)
  {
    status = listen(server_socket, 10);
    if (status < 0) 
    {
      printf("Error listen\n");
      goto Exit;
    }

    socklen_t addrlen;
    int client_socket = accept(server_socket, (struct sockaddr *) &address, &addrlen);
    printf("client_socket: %i\n", client_socket);
    if (client_socket < 0)
    {
      if (EWOULDBLOCK == errno) 
      {
        printf("Waiting for incoming connection\n");
        sleep(1);
      } 
      else 
      {
        printf("Accept error: %i\n", errno);
        sleep(1);
      }
    }
    else
    {
      printf("Client connected\n");

      int bytes = recv(client_socket, buffer, bufsize, 0);
      buffer[bytes] = 0;
      printf("%s\n", buffer);

      mysend(client_socket, "HTTP/1.1 200 OK\r\n");
      mysend(client_socket, "Content-Type: text/html\r\n\r\n");
      mysend(client_socket, "hello world 2\r\n");

      sleep(1);
      close(client_socket);
    }
  }

Exit:
  close(server_socket);

  return 0;
}
