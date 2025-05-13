#include "net_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/ip.h>

// Internal socket creation helper
static net_socket* create_socket(int domain, int type, int protocol, int* error) {
    net_socket* sock = malloc(sizeof(net_socket));
    if (!sock) {
        *error = ENOMEM;
        return NULL;
    }

    sock->fd = socket(domain, type, protocol);
    if (sock->fd < 0) {
        *error = errno;
        free(sock);
        return NULL;
    }

    // Set protocol type
    if (type == SOCK_STREAM) sock->proto = NET_PROTO_TCP;
    else if (type == SOCK_DGRAM) sock->proto = NET_PROTO_UDP;
    else if (type == SOCK_RAW) sock->proto = NET_PROTO_RAW;

    return sock;
}

net_socket* net_create_tcp_server(const char* ip, int port, int* error) {
    // Create base socket
    net_socket* sock = create_socket(AF_INET, SOCK_STREAM, 0, error);
    if (!sock) return NULL;

    // Set SO_REUSEADDR
    int reuse = 1;
    if (setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        *error = errno;
        net_close(sock);
        return NULL;
    }

    // Configure bind address
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = ip ? inet_addr(ip) : INADDR_ANY;

    // Bind and listen
    if (bind(sock->fd, (struct sockaddr*)&addr, sizeof(addr)) < 0 ||
        listen(sock->fd, 5) < 0) {
        *error = errno;
        net_close(sock);
        return NULL;
    }

    return sock;
}

net_socket* net_create_udp_endpoint(const char* ip, int port, int* error) {
    // Create base socket
    net_socket* sock = create_socket(AF_INET, SOCK_DGRAM, 0, error);
    if (!sock) return NULL;

    // Configure bind address
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = ip ? inet_addr(ip) : INADDR_ANY;

    if (bind(sock->fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        *error = errno;
        net_close(sock);
        return NULL;
    }

    return sock;
}

net_socket* net_create_raw_socket(int protocol, int* error) {
    // Create raw socket with specified protocol
    net_socket* sock = create_socket(AF_INET, SOCK_RAW, protocol, error);
    if (!sock) return NULL;

    // Enable IP header inclusion
    int on = 1;
    if (setsockopt(sock->fd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0) {
        *error = errno;
        net_close(sock);
        return NULL;
    }

    return sock;
}

net_socket* net_accept_tcp(net_socket* server, int* error) {
    if (server->proto != NET_PROTO_TCP) {
        *error = EPROTONOSUPPORT;
        return NULL;
    }

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    
    net_socket* client = malloc(sizeof(net_socket));
    if (!client) {
        *error = ENOMEM;
        return NULL;
    }

    client->fd = accept(server->fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client->fd < 0) {
        *error = errno;
        free(client);
        return NULL;
    }

    client->proto = NET_PROTO_TCP;
    memcpy(&client->address, &client_addr, sizeof(client_addr));
    
    return client;
}

int net_send(net_socket* sock, const void* data, size_t len, 
            const struct sockaddr* dest) {
    switch (sock->proto) {
        case NET_PROTO_TCP:
            return send(sock->fd, data, len, 0);
            
        case NET_PROTO_UDP:
        case NET_PROTO_RAW: {
            socklen_t addr_len = (dest != NULL) ? sizeof(struct sockaddr_in) : 0;
            return sendto(sock->fd, data, len, 0, dest, addr_len);
        }
            
        default:
            return -1;
    }
}

int net_recv(net_socket* sock, void* buffer, size_t buf_size,
            struct sockaddr* src) {
    switch (sock->proto) {
        case NET_PROTO_TCP:
            return recv(sock->fd, buffer, buf_size, 0);
            
        case NET_PROTO_UDP:
        case NET_PROTO_RAW: {
            socklen_t addr_len = sizeof(struct sockaddr_in);
            return recvfrom(sock->fd, buffer, buf_size, 0, 
                          src, src ? &addr_len : NULL);
        }
            
        default:
            return -1;
    }
}

void net_close(net_socket* sock) {
    if (sock) {
        close(sock->fd);
        free(sock);
    }
}

const char* net_strerror(int error_code) {
    return strerror(error_code);
}