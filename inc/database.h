
#define HEADER_MAGIC 0x776

typedef struct {
    unsigned int magic;
    unsigned short version;
    unsigned int count;
    unsigned int filesize;
} db_header_t;

typedef struct {
    char name[256];
    enum {
        SWORD,
        LANCE,
        AXE,
    } type;
    int hp;
    int atk;
} hero_t;

int create_header(int dbfd, db_header_t **header_out);
int validate_header(int dbfd, db_header_t **header_out);
void print_header(db_header_t *header);

int read_heroes(int dbfd, db_header_t *header, hero_t **heroes);
int list_heroes(db_header_t *dbhdr, hero_t *heroes);
int add_hero(db_header_t *dbhdr, hero_t **heroes, char *addstring);

int output_file(int dbfd, db_header_t *header, hero_t *heroes);
