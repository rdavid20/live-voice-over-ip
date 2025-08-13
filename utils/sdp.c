#include "sdp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_sdp_info(const sdp_info_t *info) {
    if (!info) return;

    printf("SDP Info:\n");
    printf("  IP: %s\n", info->ip);
    printf("  Port: %u\n", info->port);
    printf("  Payload Type: %d\n", info->payload_type);
    printf("  Codec: %s\n", info->codec);
    printf("  Sample Rate: %d\n", info->sample_rate);
    printf("  Channels: %d\n", info->channels);
}

char *generate_sdp_text(const char *ip, unsigned short port, int pt, const char *codec, int rate, int channels) {
  char *sdp = malloc(512);
  if (!sdp) {
    perror("malloc");
    return NULL;
  }

  snprintf(sdp, 512,
    "v=0\r\n"
    "o=- 0 0 IN IP4 %s\r\n"
    "s=RTP Audio Stream\r\n"
    "c=IN IP4 %s\r\n"
    "t=0 0 \r\n"
    "m=audio %u RTP/AVP %d\r\n"
    "a=rtpmap:%d %s/%d/%d\r\n",
    ip, ip, port, pt, pt, codec, rate, channels
  );

  return sdp;
}

void parse_incoming_sdp(const char *sdp, sdp_info_t *info) {
  int port, pt, rate, channels;
  char media[16], proto[16], nettype[8], addrtype[8], ip[64], codec[32];

  char sdp_copy[1024];
  strncpy(sdp_copy, sdp, sizeof(sdp_copy));
  sdp_copy[sizeof(sdp_copy)-1] = '\0';

  char *line = strtok(sdp_copy, "\r\n");

  while (line != NULL) {

    if (strlen(line) < 2) continue;

    switch (line[0]) {
      case 'm':
        if (line[1] == '=') {
          int ret = sscanf(line, "m=%15s %d %15s %d", media, &port, proto, &pt);
          if (ret != 4) {
            perror("sscanf");
            exit(EXIT_FAILURE);
          }

          if (strcmp(media, "audio") == 0) {
            info->port = (uint16_t)port;
            info->payload_type = pt;
          }
        }
        break;
      case 'c':
        if (line[1] == '=') {
          int ret = sscanf(line, "c=%7s %7s %63s", nettype, addrtype, ip);
          if (ret != 3) {
            perror("sscanf conn");
            exit(EXIT_FAILURE);
          }

          strncpy(info->ip, ip, sizeof(info->ip));
          info->ip[sizeof(info->ip)-1] = '\0';
        }
        break;
      case 'a':
        if (line[1] == '=') {
          int ret = sscanf(line, "a=rtpmap:%d %31[^/]/%d/%d", &pt, codec, &rate, &channels);
          if (ret != 4) {
            perror("sscanf attribute");
            exit(EXIT_FAILURE);
          }

          info->payload_type = pt;
          strncpy(info->codec, codec, sizeof(info->codec));
          info->codec[sizeof(info->codec)-1] = '\0';
          info->sample_rate = rate;
          info->channels = channels;
        }
        break;
      default:
        break;
    }

    line = strtok(NULL, "\r\n");
  }
}
