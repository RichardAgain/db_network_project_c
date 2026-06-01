#ifndef COMMON_HDR
#define COMMON_HDR

#include <stdint.h>

#define PROTOCOL_V 1

typedef enum {
    MSG_HELLO_REQ,
    MSG_HELLO_RES,
    MSG_LIST_HERO_REQ,
    MSG_LIST_HERO_RES,
    MSG_ADD_HERO_REQ,
    MSG_ADD_HERO_RES,
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

typedef struct {
    char data[256];
} proto_add_hero_req;

typedef struct {
	char data[256];
} proto_add_hero_res;

typedef struct {
} proto_list_hero_req;

typedef struct {
    char name[256];
    int type;
    int hp;
    int atk;
} proto_list_hero_res;

#endif
