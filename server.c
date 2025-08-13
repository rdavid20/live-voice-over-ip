#define _POSIX_C_SOURCE 199309L
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "utils/socket_utils.h"
#include "utils/sdp.h"
#include "utils/protocol.h"

void *client_thread(void *arg) {
  client_context_t *ctx = (client_context_t *)arg;
  char buffer[BUFFER_SIZE];

  /* Obtain assigned UDP port from file descriptor */
  struct sockaddr_in bound_addr;
  socklen_t addrlen = sizeof(bound_addr);
  if (getsockname(ctx->udp_fd, (struct sockaddr *)&bound_addr, &addrlen) < 0) {
    perror("getsockname");
    exit(1);
  }
  uint16_t udp_port = ntohs(bound_addr.sin_port);

  /* Send UDP port over TCP connection to client */
  char msg[64];
  snprintf(msg, sizeof(msg), "UDP_PORT %u\n", udp_port);
  send(ctx->tcp_fd, msg, strlen(msg), 0);

  /* Init sdp info */
  sdp_info_t sdp_info = {0};

  while (1) {
    ssize_t n = recv(ctx->tcp_fd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
    if (n > 0) {
      buffer[n] = '\0';

      /* Extract message type */
      message_type_t type = MSG_UNKOWN;
      for (int i = 0; i < MSG_COUNT; i++) {
        if (strncmp(buffer, message_type_str[i], 4) == 0)  {
          type = i;
          break;
        }
      }

      switch (type) {
        case MSG_SDP:
          printf("Got SDP message\n");
          char *sdp_text = buffer + 4;
          parse_incoming_sdp(sdp_text, &sdp_info);
          print_sdp_info(&sdp_info);
          break;

        default:
          printf("Got Unknown message type\n");
          break;
      }
    }

    struct sockaddr_in src_addr;
    socklen_t src_len = sizeof(src_addr);
    ssize_t m = recvfrom(ctx->udp_fd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT,
                         (struct sockaddr *)&src_addr, &src_len);
    if (m > 0) {

    }

    /* Sleep a bit to free up CPU time for other things */
    struct timespec req;
    req.tv_sec = 0;
    req.tv_nsec = 1000L * 1000L;
    if (nanosleep(&req, NULL) == -1) {
        perror("nanosleep");
    }
  }

  close(ctx->tcp_fd);
  close(ctx->udp_fd);
  free(ctx);
  return NULL;
}

int main() {
  int server_fd = create_server_socket();
  if (server_fd < 0) {
    return 1;
  }

  while (1) {
    struct sockaddr_in client_addr;
    int client_fd = server_accept_client(server_fd, &client_addr);

    printf("Got client\n");

    int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_fd < 0) {
      perror("udp socket");
      break;
    }

    struct sockaddr_in udp_addr;
    memset(&udp_addr, 0, sizeof(udp_addr));
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_port = htons(0);
    udp_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(udp_fd, (struct sockaddr *)&udp_addr, sizeof(udp_addr)) < 0) {
      perror("UDP bind failed");
      break;
    }

    client_context_t *ctx = malloc(sizeof(client_context_t));
    ctx->tcp_fd = client_fd;
    ctx->udp_fd = udp_fd;
    ctx->udp_addr = udp_addr;

    pthread_t tid;
    pthread_create(&tid, NULL, client_thread, ctx);
    pthread_detach(tid);
  }
  close (server_fd);
  return 0;
}
