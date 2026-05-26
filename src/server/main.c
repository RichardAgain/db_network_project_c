#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "database.h"
#include "defs.h"
#include "files.h"
#include "srvpoll.h"

int poll_loop() {
    char buffer[4096];

    struct sockaddr_in serverAddress = {0};
    struct sockaddr_in clientAddress = {0};
    socklen_t clientAddrLen = sizeof(clientAddress);

    int conn_fd;
    struct pollfd fds[100];

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("socket");
        return 0;
    }

    memset(fds, 0, sizeof(fds));
    fds[0].fd = serverSocket;
    fds[0].events = POLLIN;
    int nfds = 1;

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress,
             sizeof(serverAddress)) == -1) {
        perror("bind");
        return 0;
    }

    if (listen(serverSocket, BACKLOG) == -1) {
        perror("listen");
        return 0;
    }

    while (1) {
        int n_events = poll(fds, nfds, -1);

        if (fds[0].revents & POLLIN) {
            conn_fd = accept(serverSocket, (struct sockaddr *)&clientAddress,
                             &clientAddrLen);
            n_events--;

            fds[nfds].fd = conn_fd;
            fds[nfds].events = POLLIN;
            nfds++;
        }

        for (int i = 1; i <= nfds && n_events > 0; i++) {
            if ((fds[i].revents & POLLIN) && fds[i].fd != -1) {
                n_events--;

                int fd = fds[i].fd;

                memset(buffer, 0, sizeof(buffer));
                int bytes_read = read(fd, buffer, sizeof(buffer));
                if (bytes_read <= 0) {
                    printf("closing connection\n");

                    close(fd);
                    fds[i].fd = -1;
                    fds[i].events = POLLHUP;
                    nfds--;

                    continue;
                }

                handle_client_msg(fd, buffer);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    poll_loop();

    int opt;
    int dbfd = -1;

    short new_file = 0;
    short list = 0;
    char *filepath = NULL;
    char *addstring = NULL;
    int idx_to_remove = -1;

    db_header_t *db_header = NULL;
    hero_t *heroes = NULL;

    while ((opt = getopt(argc, argv, "nf:a:r:l")) != -1) {
        switch (opt) {
        case 'n':
            new_file = 1;
            break;
        case 'l':
            list = 1;
            break;
        case 'f':
            filepath = optarg;
            break;
        case 'a':
            addstring = optarg;
            break;
        case 'r':
            idx_to_remove = atoi(optarg);
            break;
        default:
            printf("incorrect usage\n");
            break;
        }
    }

    if (filepath == NULL) {
        printf("filepath is required\n");
        return 0;
    }

    if (new_file) {
        dbfd = create_db_file(filepath);
        if (dbfd == -1) {
            printf("Error creating database file\n");
            return 0;
        }

        if (create_header(dbfd, &db_header) != STATUS_OK) {
            printf("Error creating database file\n");
            return 0;
        };
    } else {
        dbfd = open_db_file(filepath);
        if (dbfd == -1) {
            printf("Error opening database file\n");
            return 0;
        }

        if (validate_header(dbfd, &db_header) != STATUS_OK) {
            printf("Error opening database file\n");
            return 0;
        };
    }

    read_heroes(dbfd, db_header, &heroes);

    if (addstring) {
        if (add_hero(db_header, &heroes, addstring) != STATUS_OK) {
            printf("Error adding hero\n");
            return 0;
        }
    }

    // if (idx_to_remove != -1) {
    //     if (remove_hero(db_header, &heroes, idx_to_remove) != STATUS_OK) {
    //         printf("Error removing hero\n");
    //         return 0;
    //     }
    // }

    if (list) {
        list_heroes(db_header, heroes);
    }

    output_file(dbfd, db_header, heroes);

    close(dbfd);
}
