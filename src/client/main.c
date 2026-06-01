#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"
#include "defs.h"
#include "srvpoll.h"

int send_list_req(int fd) {
    char buf[MAX_BUFFER] = {0};

    proto_hdr_t *hdr = (proto_hdr_t *)buf;
    hdr->type = htonl(MSG_LIST_HERO_REQ);
    hdr->len = htons(0);

    proto_list_hero_req *request = (proto_list_hero_req *)&hdr[1];

    write(fd, buf, sizeof(proto_hdr_t) + sizeof(proto_add_hero_req));

    // read msg from server
    read(fd, buf, MAX_BUFFER);

    hdr->type = ntohl(hdr->type);
    hdr->len = ntohs(hdr->len);

    if (hdr->type == MSG_ERROR) {
        printf("Protocol mismatch!\n");
        close(fd);
        return STATUS_ERROR;
    }

    printf("there are %d employees\n", hdr->len);

    if (hdr->type == MSG_LIST_HERO_RES) {
        printf("listing...\n");

        proto_list_hero_res *hero = (proto_list_hero_res *)&hdr[1];

        for (int i = 0; i < hdr->len; i++) {
            read(fd, hero, sizeof(proto_list_hero_res));

            hero->type = ntohl(hero->type);
            hero->hp = ntohl(hero->hp);
            hero->atk = ntohl(hero->atk);

            printf("Hero %d\n", i);
            printf("\t%s\n", hero->name);
            printf("\tType: %d\n", hero->type);
            printf("\tHp: %d\n", hero->hp);
            printf("\tAtk: %d\n", hero->atk);
        }
    }

    return STATUS_OK;
}

int send_add_req(int fd, char *addstring) {
    char buf[MAX_BUFFER] = {0};

    proto_hdr_t *hdr = (proto_hdr_t *)buf;
    hdr->type = htonl(MSG_ADD_HERO_REQ);
    hdr->len = htons(1);

    proto_add_hero_req *request = (proto_add_hero_req *)&hdr[1];
    memcpy(&request->data, addstring, sizeof(request->data));

    write(fd, buf, sizeof(proto_hdr_t) + sizeof(proto_add_hero_req));

    // read msg from server
    read(fd, buf, MAX_BUFFER);

    hdr->type = ntohl(hdr->type);
    hdr->len = ntohs(hdr->len);

    if (hdr->type == MSG_ERROR) {
        printf("Protocol mismatch!\n");
        close(fd);
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

int send_hello(int fd) {
    char buf[4096] = {0};

    proto_hdr_t *hdr = (proto_hdr_t *)buf;
    hdr->type = htonl(MSG_HELLO_REQ);
    hdr->len = htons(1);

    proto_hello_req *hello = (proto_hello_req *)&hdr[1];
    hello->proto_v = htons(PROTOCOL_V);

    write(fd, buf, sizeof(proto_hdr_t) + sizeof(proto_hello_req));

    // read msg from server
    read(fd, buf, sizeof(buf));

    hdr->type = ntohl(hdr->type);
    hdr->len = ntohs(hdr->len);

    if (hdr->type == MSG_ERROR) {
        printf("Protocol mismatch!\n");
        close(fd);
        return STATUS_ERROR;
    }

    printf("Successfully connected to the server, protocol v1\n");
    return STATUS_OK;
}

int main(int argc, char *argv[]) {
    char *addarg = NULL;
    char *hostarg = NULL;
    char *portarg = NULL;
    unsigned short port = 0;

    char should_list = 0;

    int opt;

    while ((opt = getopt(argc, argv, "p:h:a:l")) != -1) {
        switch (opt) {
        case 'h':
            hostarg = optarg;
            break;
        case 'p':
            portarg = optarg;
            port = atoi(portarg);
            break;
        case 'a':
            addarg = optarg;
            break;
        case 'l':
            should_list = 1;
            break;
        default:
            printf("incorrect usage\n");
            break;
        }
    }

    if (port == 0) {
        printf("Port not specified\n");
        return -1;
    }

    if (hostarg == NULL) {
        printf("Host not specified\n");
        return -1;
    }

    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(hostarg);
    serverAddress.sin_port = htons(5555);

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("socket");
        return 0;
    }

    if (connect(serverSocket, (struct sockaddr *)&serverAddress,
                sizeof(serverAddress)) == -1) {
        perror("connect");
        close(serverSocket);
        return 0;
    }

    if (send_hello(serverSocket) != STATUS_OK) {
        printf("Error connecting to server\n");
        return -1;
    };

    if (addarg) {
        if (send_add_req(serverSocket, addarg) != STATUS_OK) {
            printf("Error adding hero to database\n");
            return -1;
        }
    }

    if (should_list) {
        send_list_req(serverSocket);
    }

    close(serverSocket);
}
