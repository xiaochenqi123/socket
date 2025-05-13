/**
 * @file net_lib.h
 * @brief Cross-protocol socket library (TCP/UDP/RAW)
 */

#ifndef NET_LIB_H
#define NET_LIB_H

#include <stddef.h>
#include <sys/socket.h>

typedef enum {
    NET_PROTO_TCP = 0,  ///< Stream-oriented TCP
    NET_PROTO_UDP,      ///< Datagram-oriented UDP 
    NET_PROTO_RAW       ///< Raw packet access (requires root)
} net_protocol_type;

typedef struct net_socket {
    int fd;                     ///< Socket file descriptor
    net_protocol_type proto;    ///< Protocol type identifier
    struct sockaddr_in address; ///< Bound address information
} net_socket;

/**
 * @brief Creates a TCP server socket
 * @param ip Bind IP address (NULL for any)
 * @param port Bind port number
 * @param error Output error code
 * @return Socket handle or NULL on failure
 */
net_socket* net_create_tcp_server(const char* ip, int port, int* error);

/**
 * @brief Creates a UDP communication endpoint
 * @param ip Bind IP address (NULL for any)
 * @param port Bind port number
 * @param error Output error code
 * @return Socket handle or NULL on failure
 */
net_socket* net_create_udp_endpoint(const char* ip, int port, int* error);

/**
 * @brief Creates a raw socket for packet-level access
 * @param protocol IPPROTO_* value (e.g., IPPROTO_ICMP)
 * @param error Output error code
 * @return Socket handle or NULL on failure
 */
net_socket* net_create_raw_socket(int protocol, int* error);

/**
 * @brief Accepts incoming TCP connection
 * @param server TCP server socket
 * @param error Output error code
 * @return Client socket handle or NULL on failure
 */
net_socket* net_accept_tcp(net_socket* server, int* error);

/**
 * @brief Sends data through the socket
 * @param sock Socket handle
 * @param data Data buffer to send
 * @param len Data length
 * @param dest Destination address (NULL for connected sockets)
 * @return Bytes sent or -1 on error
 */
int net_send(net_socket* sock, const void* data, size_t len, 
            const struct sockaddr* dest);

/**
 * @brief Receives data from the socket
 * @param sock Socket handle
 * @param buffer Receive buffer
 * @param buf_size Buffer capacity
 * @param src Output source address (optional)
 * @return Bytes received, 0 on closure, or -1 on error
 */
int net_recv(net_socket* sock, void* buffer, size_t buf_size,
            struct sockaddr* src);

/**
 * @brief Closes socket and releases resources
 * @param sock Socket handle to close
 */
void net_close(net_socket* sock);

/**
 * @brief Converts error code to human-readable string
 * @param error_code System error code
 * @return Error description
 */
const char* net_strerror(int error_code);

#endif // NET_LIB_H