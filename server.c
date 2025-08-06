#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "utils/socket_utils.h"

int main() {
  int server_fd = create_server_socket(TCP_PORT);
  if (server_fd < 0) {
    return 1;
  }

  struct sockaddr_in client_addr;
  int client_fd = server_accept_client(server_fd, &client_addr);
  if (client_fd < 0) {
    return 1;
  }

  char buffer[BUFFER_SIZE] = {0};
  read(client_fd, buffer, BUFFER_SIZE);
  printf("Received from client: %s\n", buffer);

  const char* response = "Hello from server";
  send(client_fd, response, strlen(response), 0);

  close(client_fd);
  close(server_fd);

  return 0;
}
