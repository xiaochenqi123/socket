#ifndef NET_LIB_H
#define NET_LIB_H

#include <stddef.h>

typedef struct net_socket net_socket;

typedef enum{
    NET_PROTO_TCP=0,
    NET_PROTO_UDP,
    NET_PROTO_RAW
}net_protocol_type;

struct net_socket {
    int fd;
    net_protocol_type proto;
    struct sockaddr_in addr;
};

/*Create a server*/
net_socket* net_server_create(const char* ip, int port, int* error);

/* Client connection */
net_socket* net_client_connect(const char* ip, int port, int* error);

/*Accept connection (server side)*/
net_socket* net_accept(net_socket* server, int* error);

/*Send data (blocking mode)*/
int net_send(net_socket* sock, const void* data, size_t len);

/*Receive data (blocking mode)*/ 
int net_recv(net_socket* sock, void* buffer, size_t buf_size);

/*Close Connection*/ 
void net_close(net_socket* sock);

/*Convert error code to string*/ 
const char* net_strerror(int error_code);

#endif

