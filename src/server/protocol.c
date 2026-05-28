#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"
#include "defs.h"
#include "files.h"
#include "srvpoll.h"

void send_add_msg(int client_fd, proto_hdr_t *hdr, char* addstring) {
    hdr->type = htonl(MSG_HELLO_RES);
    hdr->len = htons(1);
    proto_add_hero_res *request = (proto_add_hero_res *)&hdr[1];
    strncpy(request->data, addstring, strlen(addstring));

    write(client_fd, hdr, sizeof(proto_hdr_t) + sizeof(proto_hello_res));
}

void send_hello_msg(int client_fd, proto_hdr_t *hdr) {
    hdr->type = htonl(MSG_HELLO_RES);
    hdr->len = htons(1);
    proto_hello_res *data = (proto_hello_res *)&hdr[1];
    data->proto_v = htons(PROTOCOL_V);

    write(client_fd, hdr, sizeof(proto_hdr_t) + sizeof(proto_hello_res));
}

void send_error_msg(int client_fd, proto_hdr_t *hdr) {
    hdr->type = htonl(MSG_ERROR);
    hdr->len = htons(0);

    write(client_fd, hdr, sizeof(proto_hdr_t));
}

void handle_client_msg(int dbfd, db_header_t *db_header, hero_t **heroes, client_state_t *client) {
    proto_hdr_t *header = (proto_hdr_t *)client->buffer;

    header->type = ntohl(header->type);
    header->len = ntohs(header->len);

    if (client->state == STATE_HELLO) {
        if (header->type != MSG_HELLO_REQ) {
            printf("Not a valid request\n");
            send_error_msg(client->fd, header);
            return;
        }

        proto_hello_req *hello = (proto_hello_req *)&header[1];
        hello->proto_v = ntohs(hello->proto_v);

        if (hello->proto_v != PROTOCOL_V) {
            printf("Protocol mismatch!\n");
            send_error_msg(client->fd, header);
            return;
        }

        printf("client sent hello!\n");
        client->state = STATE_MSG;
        send_hello_msg(client->fd, header);
    }

    if (client->state == STATE_MSG) {
        if (header->type == MSG_ADD_HERO_REQ) {
            proto_add_hero_req *request = (proto_add_hero_req *)&header[1];


            if (add_hero(db_header, heroes, (char*)request->data) == STATUS_ERROR) {
                printf("Error adding hero to database\n");
                send_error_msg(client->fd, header);
            }

            send_add_msg(client->fd, header, (char*)request->data);
            printf("Succesfully added hero\n");

            output_file(dbfd, db_header, *heroes);
        }

        if (header->type == MSG_LIST_HERO_REQ) {
        }
    }
}

void init_clients(client_state_t *clients) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].fd = -1;
        clients[i].state = STATE_NEW;
        memset(&clients[i].buffer, 0, MAX_BUFFER);
    }
}

int find_free_slot(client_state_t *clients) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd == -1) {
            return i;
        }
    }
    return -1;
}

int find_slot_by_fd(client_state_t *clients, int to_find) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd == to_find) {
            return i;
        }
    }
    return -1;
}
