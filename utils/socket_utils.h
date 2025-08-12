#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#define UDP_PORT 5004
#define TCP_PORT 5003
#define BUFFER_SIZE 1024

#include <arpa/inet.h>

typedef struct {
  int tcp_fd;
  int udp_fd;
  struct sockaddr_in udp_addr;
} client_context_t;

int create_connected_socket(const char* ip_addr);
int create_server_socket();
int server_accept_client(int server_fd, struct sockaddr_in* client_addr);

#endif // SOCKET_UTILS_H
