// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Body { char *line; struct Body *next; } Body;
typedef struct Record { char *title; Body *body; struct Record *next; } Record;
Record *catalog = NULL;

Record *find_record(const char *title) {
    for (Record *r = catalog; r; r = r->next)
        if (strcmp(r->title, title) == 0) return r;
    return NULL;
}

void add_record(const char *title, Body *body) {
    if (find_record(title)) {
        // delete existing body
        while (body) { Body *b = body; body = b->next; free(b->line); free(b); }
        return;
    }
    Record *r = malloc(sizeof(Record));
    if (!r) return;
    r->title = strdup(title); r->body = body; r->next = catalog; catalog = r;
}

void delete_record(const char *title) {
    Record *prev = NULL;
    for (Record *r = catalog; r; prev = r, r = r->next) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next; else catalog = r->next;
            free(r->title);
            while (r->body) { Body *b = r->body; r->body = b->next; free(b->line); free(b); }
            free(r);
            return;
        }
    }
}

void find_record_print(const char *title) {
    Record *r = find_record(title);
    if (!r) { printf("NOT_FOUND\n"); return; }
    printf("%s\n", r->title);
    for (Body *b = r->body; b; b = b->next) printf("%s\n", b->line);
}

void print_catalog(void) {
    for (Record *r = catalog; r; r = r->next) {
        printf("%s\n", r->title);
        for (Body *b = r->body; b; b = b->next) printf("%s\n", b->line);
        printf("--\n");
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            char title[4096] = "";
            Body *head = NULL, *tail = NULL;
            int in_record = 0;
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) {
                    if (in_record) {
                        add_record(title, head);
                        title[0] = 0;
                        head = tail = NULL;
                        in_record = 0;
                    }
                    continue;
                }
                if (!in_record) {
                    strcpy(title, line);
                    head = tail = NULL;
                    in_record = 1;
                } else {
                    Body *b = malloc(sizeof(Body));
                    if (b) {
                        b->line = strdup(line); b->next = NULL;
                        if (tail) tail->next = b; else head = b;
                        tail = b;
                    }
                }
            }
            if (in_record) {
                add_record(title, head);
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "FIND") == 0) {
            char title[4096];
            strcpy(title, rest);
            find_record_print(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char title[4096];
            strcpy(title, rest);
            delete_record(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            char title[4096];
            strcpy(title, rest);
            // read body lines until "--"
            Body *head = NULL, *tail = NULL;
            while (fgets(line, sizeof(line), stdin)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) break;
                Body *b = malloc(sizeof(Body));
                if (b) {
                    b->line = strdup(line); b->next = NULL;
                    if (tail) tail->next = b; else head = b;
                    tail = b;
                }
            }
            add_record(title, head);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_catalog();
        }
    }
    while (catalog) { Record *r = catalog; catalog = r->next; free(r->title); while (r->body) { Body *b = r->body; r->body = b->next; free(b->line); free(b); } free(r); }
    return 0;
}