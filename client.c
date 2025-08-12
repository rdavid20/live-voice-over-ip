#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "utils/socket_utils.h"


int main()
{
  char buffer[BUFFER_SIZE];

  int sock = create_connected_socket("127.0.0.1");
  if (sock < 0) {
    return 1;
  }

  ssize_t n = read(sock, buffer, BUFFER_SIZE - 1);
  buffer[n] = '\0';

  uint16_t udp_port;
  if (sscanf(buffer, "UDP_PORT %hu", &udp_port) != 1) {
    perror("sscanf udp port");
  }

  printf("Received: %u\n", udp_port);

  int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (udp_fd < 0) {
    perror("udp socket");
    return 1;
  }

  struct sockaddr_in server_udp_addr;
  memset(&server_udp_addr, 0, sizeof(server_udp_addr));
  server_udp_addr.sin_family = AF_INET;
  server_udp_addr.sin_port = htons(udp_port);
  inet_pton(AF_INET, "127.0.0.1", &server_udp_addr.sin_addr);

  const char* payload = "hello on udp";
  ssize_t sent = sendto(udp_fd, payload, strlen(payload), 0,
    (struct sockaddr *)&server_udp_addr, sizeof(server_udp_addr));

  if (sent < 0) {
    perror("sendto");
  }

  close(sock);
  return 0;
}
