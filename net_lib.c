#include "net_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

/*Create a server*/
net_socket* net_server_create(const char* ip, int port, int* error){
    net_socket* sock=malloc(sizeof(net_socket));
    if(!sock){
        if(error) *error = ENOMEM;
        return NULL;
    }
    if((sock->fd=socket(AF_INET,SOCK_STREAM,0))<0){
        if (error) *error = errno;
        free(sock);
        return NULL;
    }
}
