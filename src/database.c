#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "database.h"
#include "defs.h"

void print_header(db_header_t *header) {
    printf("magic: %d\n", header->magic);
    printf("version: %d\n", header->version);
    printf("count: %d\n", header->count);
    printf("filesize: %d\n", header->filesize);
}

int create_header(int dbfd, db_header_t **header_out) {
    db_header_t *new_header = calloc(1, sizeof(db_header_t));
    new_header->magic = HEADER_MAGIC;
    new_header->version = 1;
    new_header->count = 0;
    new_header->filesize = 0;

    *header_out = new_header;

    return STATUS_OK;
}

int validate_header(int dbfd, db_header_t **header_out) {
    if (dbfd == -1) {
        printf("Not a valid file descriptor\n");
        return STATUS_ERROR;
    }

    db_header_t *new_header = calloc(1, sizeof(db_header_t));
    if (new_header == NULL) {
        return STATUS_ERROR;
    }

    int bytes_read = read(dbfd, new_header, sizeof(db_header_t));
    new_header->magic = ntohl(new_header->magic);
    new_header->version = ntohs(new_header->version);
    new_header->count = ntohl(new_header->count);
    new_header->filesize = ntohl(new_header->filesize);

    if (new_header->version != 1) {
        printf("version not supported\n");
        return STATUS_ERROR;
    }

    if (new_header->magic != HEADER_MAGIC) {
        printf("filetype not supported\n");
        return STATUS_ERROR;
    }

    *header_out = new_header;

    return STATUS_OK;
}

int read_heroes(int dbfd, db_header_t *dbhdr, hero_t **heroes) {
    if (dbfd == -1) {
        printf("Not a valid file descriptor\n");
        return STATUS_ERROR;
    }

    int count = dbhdr->count;
    hero_t *new_heroes = calloc(count, sizeof(hero_t));

    for (int i = 0; i < count; i++) {
        read(dbfd, &new_heroes[i], sizeof(hero_t));

        new_heroes[i].type = ntohl(new_heroes[i].type);
        new_heroes[i].hp = ntohl(new_heroes[i].hp);
        new_heroes[i].atk = ntohl(new_heroes[i].atk);
    }

    *heroes = new_heroes;

    return STATUS_OK;
}

int add_hero(db_header_t *dbhdr, hero_t **heroes, char *addstring) {
    char *name = strtok(addstring, ",");
    char *hp = strtok(NULL, ",");
    char *atk = strtok(NULL, ",");

    hero_t *h = NULL;
    h = realloc(*heroes, (dbhdr->count + 1) * sizeof(hero_t));
    if (h == NULL) {
        return STATUS_ERROR;
    }

    int idx = dbhdr->count;
    dbhdr->count++;

    strncpy(h[idx].name, name, sizeof(h[idx].name) - 1);
    h[idx].type = SWORD;
    h[idx].hp = atoi(hp);
    h[idx].atk = atoi(atk);

    *heroes = h;

    return STATUS_OK;
}

int list_heroes(db_header_t *dbhdr, hero_t *heroes) {
    for (int i = 0; i < dbhdr->count; i++) {
        printf("Hero %d\n", i);
        printf("\t%s\n", heroes[i].name);
        printf("\tType: %d\n", heroes[i].type);
        printf("\tHp: %d\n", heroes[i].hp);
        printf("\tAtk: %d\n", heroes[i].atk);
    }

    return STATUS_OK;
}

int output_file(int dbfd, db_header_t *dbhdr, hero_t *heroes) {
    if (dbfd == -1) {
        printf("Not a valid file descriptor\n");
        return STATUS_ERROR;
    }

    int count = dbhdr->count;

    dbhdr->magic = htonl(dbhdr->magic);
    dbhdr->version = htons(dbhdr->version);
    dbhdr->count = htonl(dbhdr->count);
    dbhdr->filesize = htonl(dbhdr->filesize);

    lseek(dbfd, 0, SEEK_SET);
    write(dbfd, dbhdr, sizeof(db_header_t));

    for (int i = 0; i < count; i++) {
        hero_t to_write = {0};

        strncpy(to_write.name, heroes[i].name, sizeof(to_write.name) - 1);
        to_write.type = htonl(heroes[i].type);
        to_write.hp = htonl(heroes[i].hp);
        to_write.atk = htonl(heroes[i].atk);

        write(dbfd, &to_write, sizeof(hero_t));
    }

    return STATUS_OK;
}
