// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Body {
    char *line;
    struct Body *next;
} Body;

typedef struct Record {
    char *title;
    Body *bodies;
    Body *tail;
    struct Record *next;
} Record;

Record *records = NULL;

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    Record *curr = NULL;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "--") == 0) {
            curr = NULL;
        } else if (!curr) {
            curr = malloc(sizeof(Record));
            curr->title = strdup(line);
            curr->bodies = NULL;
            curr->tail = NULL;
            curr->next = records;
            records = curr;
        } else {
            Body *b = malloc(sizeof(Body));
            b->line = strdup(line);
            b->next = NULL;
            if (curr->tail) curr->tail->next = b;
            else curr->bodies = b;
            curr->tail = b;
        }
    }
    fclose(f);
}

Record *find_record(const char *title) {
    Record *curr = records;
    while (curr) {
        if (strcmp(curr->title, title) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void add_record(const char *title) {
    if (find_record(title)) return;
    Record *r = malloc(sizeof(Record));
    r->title = strdup(title);
    r->bodies = NULL;
    r->tail = NULL;
    r->next = records;
    records = r;
    
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "--") == 0) break;
        Body *b = malloc(sizeof(Body));
        b->line = strdup(line);
        b->next = NULL;
        if (r->tail) r->tail->next = b;
        else r->bodies = b;
        r->tail = b;
    }
}

void delete_record(const char *title) {
    Record *curr = records, *prev = NULL;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else records = curr->next;
            Body *b = curr->bodies;
            while (b) {
                Body *next = b->next;
                free(b->line);
                free(b);
                b = next;
            }
            free(curr->title);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_record_print(const char *title) {
    Record *r = find_record(title);
    if (!r) return;
    printf("%s\n", r->title);
    Body *b = r->bodies;
    while (b) {
        printf("%s\n", b->line);
        b = b->next;
    }
}

void print_records() {
    Record *curr = records;
    while (curr) {
        printf("%s\n", curr->title);
        Body *b = curr->bodies;
        while (b) {
            printf("%s\n", b->line);
            b = b->next;
        }
        curr = curr->next;
    }
}

void free_all() {
    Record *curr = records;
    while (curr) {
        Record *next = curr->next;
        Body *b = curr->bodies;
        while (b) {
            Body *next_b = b->next;
            free(b->line);
            free(b);
            b = next_b;
        }
        free(curr->title);
        free(curr);
        curr = next;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32], title[256] = {0};
        sscanf(line, "%31s %255[^\n]", cmd, title);
        
        if (strcmp(cmd, "FIND") == 0) find_record_print(title);
        else if (strcmp(cmd, "DELETE") == 0) delete_record(title);
        else if (strcmp(cmd, "ADD") == 0) add_record(title);
        else if (strcmp(cmd, "PRINT") == 0) print_records();
    }
    free_all();
    return 0;
}