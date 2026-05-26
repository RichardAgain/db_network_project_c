#ifndef SRVPOLL_HDR
#define SRVPOLL_HDR

#define PORT 5555
#define BACKLOG 10

void handle_client_msg(int client_fd, void *client_data);

#endif
