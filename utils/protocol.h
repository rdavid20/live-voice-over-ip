#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>


typedef enum {
  MSG_SDP = 0,
  MSG_COUNT,
  MSG_UNKOWN,
} message_type_t;

static const char *message_type_str[] = {
    [MSG_SDP] = "SDP ",
};

#define MSG_TYPE_LEN 4
#define MAX_TCP_MSG_SIZE 1024

#endif
