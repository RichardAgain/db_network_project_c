#include <stdio.h>
#include <unistd.h>

#include <string.h>

#include "database.h"
#include "defs.h"
#include "files.h"

int main(int argc, char *argv[]) {
    int opt;
    int dbfd = -1;

    short new_file = 0;
    short list = 0;
    char *filepath = NULL;
    char *addstring = NULL;

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

    if (list) {
        list_heroes(db_header, heroes);
    }

    output_file(dbfd, db_header, heroes);

    close(dbfd);
}
