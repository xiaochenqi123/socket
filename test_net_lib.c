#include "net_lib.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define TEST_PORT 8080
#define TEST_MSG "Hello Network Library!"

void test_tcp() {
    int err = 0;
    printf("\n=== Testing TCP Protocol ===\n");
    
    // Server setup
    net_socket* server = net_create_tcp_server("127.0.0.1", TEST_PORT, &err);
    if (!server) {
        printf("TCP Server create failed: %s\n", net_strerror(err));
        return;
    }
    printf("TCP Server created on port %d\n", TEST_PORT);

    // Client connect
    net_socket* client = net_create_udp_endpoint("127.0.0.1", 0, &err); // 故意使用错误协议
    if (client) {
        printf("ERROR: Should reject non-TCP client\n");
        net_close(client);
    }
    
    client = net_create_tcp_server("127.0.0.1", TEST_PORT, &err); // 端口冲突测试
    if (client) {
        printf("ERROR: Port conflict test failed\n");
        net_close(client);
    }

    // Valid client
    client = net_create_udp_endpoint("127.0.0.1", 0, &err);
    if (!client) {
        printf("TCP Client create failed: %s\n", net_strerror(err));
        net_close(server);
        return;
    }

    // Send/Receive test
    if (net_send(client, TEST_MSG, strlen(TEST_MSG), NULL) < 0) {
        printf("TCP Send failed: %s\n", net_strerror(errno));
    }

    char buffer[256];
    int received = net_recv(client, buffer, sizeof(buffer), NULL);
    if (received > 0) {
        buffer[received] = '\0';
        printf("TCP Received: %s\n", buffer);
    }

    net_close(client);
    net_close(server);
}

void test_udp() {
    printf("\n=== Testing UDP Protocol ===\n");
    int err = 0;
    
    net_socket* server = net_create_udp_endpoint("127.0.0.1", TEST_PORT, &err);
    if (!server) {
        printf("UDP Server create failed: %s\n", net_strerror(err));
        return;
    }
    printf("UDP Server ready on port %d\n", TEST_PORT);

    net_socket* client = net_create_udp_endpoint("127.0.0.1", 0, &err);
    if (!client) {
        printf("UDP Client create failed: %s\n", net_strerror(err));
        net_close(server);
        return;
    }

    // Build destination address
    struct sockaddr_in dest = {
        .sin_family = AF_INET,
        .sin_port = htons(TEST_PORT),
        .sin_addr.s_addr = inet_addr("127.0.0.1")
    };

    if (net_send(client, TEST_MSG, strlen(TEST_MSG), (struct sockaddr*)&dest) < 0) {
        printf("UDP Send failed: %s\n", net_strerror(errno));
    }

    char buffer[256];
    struct sockaddr_in src_addr;
    int received = net_recv(server, buffer, sizeof(buffer), (struct sockaddr*)&src_addr);
    if (received > 0) {
        buffer[received] = '\0';
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &src_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("UDP Received from %s:%d: %s\n", 
              client_ip, ntohs(src_addr.sin_port), buffer);
    }

    net_close(client);
    net_close(server);
}

void test_raw() {
    printf("\n=== Testing RAW Socket ==\n");
    int err = 0;
    
    net_socket* raw = net_create_raw_socket(IPPROTO_ICMP, &err);
    if (!raw) {
        printf("RAW socket create failed: %s (可能需要root权限)\n", net_strerror(err));
        return;
    }
    printf("RAW socket created for ICMP protocol\n");
    net_close(raw);
}

int main() {
    test_tcp();
    test_udp();
    test_raw();
    return 0;
}