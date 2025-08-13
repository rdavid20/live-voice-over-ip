#ifndef SDP_H
#define SDP_H

#include <stdint.h>

typedef struct {
  char ip[64];
  uint16_t port;
  int payload_type;
  char codec[32];
  int sample_rate;
  int channels;
} sdp_info_t;

char *generate_sdp_text(const char *ip, unsigned short port, int pt, const char *codec, int rate, int channels);
void parse_incoming_sdp(const char *sdp, sdp_info_t *info);
void print_sdp_info(const sdp_info_t *info);

#endif
