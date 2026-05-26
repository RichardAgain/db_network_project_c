#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"
#include "database.h"
#include "defs.h"
#include "files.h"

void send_hello_msg(int client_fd, proto_hdr_t *hdr) {
    hdr->type = htonl(MSG_HELLO_REQ);
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

void handle_client_msg(int client_fd, void *client_data) {
    proto_hdr_t *header = (proto_hdr_t *)client_data;

    header->type = ntohl(header->type);
    header->len = ntohs(header->len);

    if (header->type == MSG_HELLO_REQ) {
        proto_hello_req *hello = (proto_hello_req *)&header[1];
        hello->proto_v = ntohs(hello->proto_v);

        if (hello->proto_v != PROTOCOL_V) {
            printf("Protocol mismatch!\n");
            send_error_msg(client_fd, header);
            return;
        }

        printf("client sent hello!\n");
        send_hello_msg(client_fd, header);
    } else {
    	printf("Not a valid protocol header\n");
    }
}
