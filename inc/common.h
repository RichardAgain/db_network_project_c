#ifndef COMMON_HDR
#define COMMON_HDR

#include <stdint.h>

#define PROTOCOL_V 1

typedef enum {
    MSG_HELLO_REQ,
    MSG_HELLO_RES,
    MSG_ERROR,
} proto_type_e;

typedef struct {
	proto_type_e type;
	uint16_t len;
} proto_hdr_t;

typedef struct {
    uint16_t proto_v;
} proto_hello_req;

typedef struct {
    uint16_t proto_v;
} proto_hello_res;

#endif
