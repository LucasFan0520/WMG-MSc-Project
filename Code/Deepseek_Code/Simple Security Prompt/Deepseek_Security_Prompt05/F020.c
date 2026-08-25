/* F020.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct BodyLine {
    char *text;
    struct BodyLine *next;
} BodyLine;

typedef struct Record {
    char *title;
    BodyLine *body;
    struct Record *next;
} Record;

Record *catalog = NULL;

Record *find_record(const char *title) {
    Record *r = catalog;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void delete_record(Record *r) {
    free(r->title);
    while (r->body) {
        BodyLine *b = r->body;
        r->body = r->body->next;
        free(b->text);
        free(b);
    }
    free(r);
}

void load_records(FILE *f) {
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) continue;
        char title[256];
        strcpy(title, line);
        Record *r = malloc(sizeof(Record));
        if (!r) continue;
        r->title = strdup(title);
        r->body = NULL;
        BodyLine *tail = NULL;
        while (fgets(line, sizeof(line), f)) {
            line[strcspn(line, "\n")] = '\0';
            if (strcmp(line, "--") == 0) break;
            BodyLine *b = malloc(sizeof(BodyLine));
            if (!b) break;
            b->text = strdup(line);
            b->next = NULL;
            if (tail) tail->next = b;
            else r->body = b;
            tail = b;
        }
        r->next = catalog;
        catalog = r;
    }
}

void find_record_print(const char *title) {
    Record *r = find_record(title);
    if (!r) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", r->title);
    BodyLine *b = r->body;
    while (b) {
        printf("%s\n", b->text);
        b = b->next;
    }
}

void delete_record_by_title(const char *title) {
    Record *r = catalog, *prev = NULL;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else catalog = r->next;
            delete_record(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void add_record(const char *title) {
    if (find_record(title)) return;
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->title = strdup(title);
    r->body = NULL;
    BodyLine *tail = NULL;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "--") == 0) break;
        BodyLine *b = malloc(sizeof(BodyLine));
        if (!b) break;
        b->text = strdup(line);
        b->next = NULL;
        if (tail) tail->next = b;
        else r->body = b;
        tail = b;
    }
    r->next = catalog;
    catalog = r;
}

void print_all(void) {
    Record *r = catalog;
    while (r) {
        printf("%s\n", r->title);
        BodyLine *b = r->body;
        while (b) {
            printf("%s\n", b->text);
            b = b->next;
        }
        r = r->next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            load_records(f);
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg[256];
        int n = sscanf(line, "%15s %255s", cmd, arg);
        if (n < 2) continue;
        if (strcmp(cmd, "FIND") == 0) {
            find_record_print(arg);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_record_by_title(arg);
        } else if (strcmp(cmd, "ADD") == 0) {
            add_record(arg);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    while (catalog) {
        Record *tmp = catalog;
        catalog = catalog->next;
        delete_record(tmp);
    }
    return 0;
}