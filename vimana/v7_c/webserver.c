/*
Code below based on this blog post by Manula:
http://blog.manula.org/2011/05/writing-simple-web-server-in-c.html

Additional links:
https://www.jmarshall.com/easy/http/
https://github.com/ankushagarwal/nweb
https://stackoverflow.com/questions/176409/build-a-simple-http-server-in-c
https://www.freebsd.org/cgi/man.cgi?query=send&sektion=2&manpath=freebsd-release-ports
https://www.freebsd.org/cgi/man.cgi?query=recv&sektion=2&manpath=freebsd-release-ports
https://tldp.org/LDP/lpg/node12.html
https://www.irva.org
*/


#include<netinet/in.h>    
#include<stdio.h>    
#include<stdlib.h>    
#include<sys/socket.h>    
#include<sys/stat.h>    
#include<sys/types.h>    
#include<unistd.h>    
    
int main() {    
   int create_socket, new_socket;    
   socklen_t addrlen;    
   int bufsize = 4000;    
   char *buffer = malloc(bufsize);    
   struct sockaddr_in address;    
 
   if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0){    
      printf("The socket was created\n");
   }
    
   address.sin_family = AF_INET;    
   address.sin_addr.s_addr = INADDR_ANY;    
   address.sin_port = htons(8888);    
    
   if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) == 0){    
      printf("Binding Socket\n");
   }
    
    
   while (1) {    
      if (listen(create_socket, 10) < 0) {    
         perror("server: listen");    
         exit(1);    
      }    
    
      if ((new_socket = accept(create_socket, (struct sockaddr *) &address, &addrlen)) < 0) {    
         perror("server: accept");    
         exit(1);    
      }    
    
      if (new_socket > 0){    
         printf("The Client is connected...\n");
      }
        
      int bytes = recv(new_socket, buffer, bufsize, 0);  
      buffer[bytes] = 0;  
      printf("%s\n", buffer);  

      send(new_socket, "HTTP/1.1 200 OK\n", 16, 0);
      send(new_socket, "Content-Type: text/html\n\n", 25, 0);
      send(new_socket, "hello world\n", 12, 0);  
/*
write(new_socket, "HTTP/1.1 200 OK\n", 16);
write(new_socket, "Content-length: 46\n", 19);
write(new_socket, "Content-Type: text/html\n\n", 25);
write(new_socket, "<html><body><H1>Hello world</H1></body></html>",46);
 */
      close(new_socket);    
   }    
   close(create_socket);    
   return 0;    
}
