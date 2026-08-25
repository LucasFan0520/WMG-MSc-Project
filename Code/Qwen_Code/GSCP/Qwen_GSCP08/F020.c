// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct BodyLine {
    char *text;
    struct BodyLine *next;
} BodyLine;

typedef struct Catalog {
    char *title;
    BodyLine *body;
    struct Catalog *next;
} Catalog;

Catalog *head = NULL;

Catalog *find_catalog(char *title) {
    for (Catalog *c = head; c; c = c->next) {
        if (strcmp(c->title, title) == 0) return c;
    }
    return NULL;
}

void add_catalog() {
    char title[1000];
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = 0;
    if (find_catalog(title)) return;
    Catalog *c = malloc(sizeof(Catalog));
    c->title = strdup(title);
    c->body = NULL;
    c->next = head;
    head = c;
    BodyLine **last = &c->body;
    char line[1000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "--") == 0) break;
        BodyLine *b = malloc(sizeof(BodyLine));
        b->text = strdup(line);
        b->next = NULL;
        *last = b;
        last = &b->next;
    }
}

void delete_catalog(char *title) {
    Catalog *prev = NULL;
    Catalog *curr = head;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->title);
            BodyLine *b = curr->body;
            while (b) {
                BodyLine *next = b->next;
                free(b->text);
                free(b);
                b = next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_catalog_body(char *title) {
    Catalog *c = find_catalog(title);
    if (c) {
        puts(c->title);
        for (BodyLine *b = c->body; b; b = b->next) {
            puts(b->text);
        }
    }
}

void print_catalogs() {
    for (Catalog *c = head; c; c = c->next) {
        puts(c->title);
        for (BodyLine *b = c->body; b; b = b->next) {
            puts(b->text);
        }
        puts("--");
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (file) {
            char line[1000];
            while (fgets(line, sizeof(line), file)) {
                line[strcspn(line, "\n")] = 0;
                Catalog *c = malloc(sizeof(Catalog));
                c->title = strdup(line);
                c->body = NULL;
                c->next = NULL;
                Catalog **last = &head;
                while (*last) last = &(*last)->next;
                *last = c;
                BodyLine **bl = &c->body;
                while (fgets(line, sizeof(line), file)) {
                    line[strcspn(line, "\n")] = 0;
                    if (strcmp(line, "--") == 0) break;
                    BodyLine *b = malloc(sizeof(BodyLine));
                    b->text = strdup(line);
                    b->next = NULL;
                    *bl = b;
                    bl = &b->next;
                }
            }
            fclose(file);
        }
    }
    char cmd[20], title[1000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "FIND") == 0) {
            scanf("%999s", title);
            find_catalog_body(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%999s", title);
            delete_catalog(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            add_catalog();
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_catalogs();
        }
    }
    return 0;
}