#include <fcntl.h>
#include <stdio.h>

int create_db_file(char *filepath) {
	int fd = open(filepath, O_RDWR, 0644);
    if (fd != -1) {
    	printf("File already exists\n");
        return -1;
    }

    fd = open(filepath, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    return fd;
}

int open_db_file(char *filepath, short *new_file) {
	int fd = open(filepath, O_RDWR, 0644);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    return fd;
}
