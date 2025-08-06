#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "utils/socket_utils.h"


int main()
{
  int sock = create_connected_socket("127.0.0.1", TCP_PORT);
  if (sock < 0) {
    return 1;
  }

  const char* msg = "Hello from client";
  send(sock, msg, strlen(msg), 0);

  char buffer[BUFFER_SIZE] = { 0 };
  read(sock, buffer, BUFFER_SIZE);
  printf("Received: %s\n", buffer);

  close(sock);

  return 0;
}
