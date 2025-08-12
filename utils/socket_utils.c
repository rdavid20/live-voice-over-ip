#include "socket_utils.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

/* CLIENT RELATED */
int create_connected_socket(const char* ip_addr) {
  int client_fd;
  struct sockaddr_in serv_addr;

  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket creatin error");
    return -1;
  }

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(TCP_PORT);

  if (inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr) <= 0) {
    perror("Invalid addr");
    close(client_fd);
    return -1;
  }

  if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Connection failed");
    close(client_fd);
    return -1;
  }

  return client_fd;
}

/* SERVER RELATED */
int create_server_socket() {
  int server_fd;
  struct sockaddr_in addr;

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket failed");
    exit(EXIT_FAILURE);
  }

  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(TCP_PORT);

  if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    perror("bind failed");
    close(server_fd);
    return -1;
  }

  if (listen(server_fd, 3) < 0) {
    perror("listen");
    close(server_fd);
    return -1;
  }

  return server_fd;
}

int server_accept_client(int server_fd, struct sockaddr_in* client_addr) {
  socklen_t addr_len = sizeof(*client_addr);
  int new_socket = accept(server_fd, (struct sockaddr*)client_addr, &addr_len);

  if (new_socket < 0) {
    perror("Accept failed");
    return -1;
  }

  return new_socket;
}
