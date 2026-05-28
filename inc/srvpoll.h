#ifndef SRVPOLL_HDR
#define SRVPOLL_HDR

#include "database.h"

#define PORT 5555
#define BACKLOG 10
#define MAX_CLIENTS 100
#define MAX_BUFFER 4096

typedef enum {
    STATE_NEW,
    STATE_CONNECTED,
    STATE_DISCONNECTED,
    STATE_HELLO,
    STATE_MSG,
} state_e;

typedef struct {
    int fd;
    state_e state;
    char buffer[MAX_BUFFER];
} client_state_t;

void init_clients(client_state_t *clients);
int find_free_slot(client_state_t *clients);
int find_slot_by_fd(client_state_t *clients, int to_find);

void handle_client_msg(int dbfd, db_header_t *db_header, hero_t **heroes, client_state_t *client);

#endif
