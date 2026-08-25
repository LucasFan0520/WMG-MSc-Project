// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Chunk {
    char *title;
    char **body;
    int body_count;
    struct Chunk *next;
} Chunk;

Chunk *head = NULL;

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        return;
    }
    char line[1000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "--") == 0) {
            continue;
        }
        Chunk *c = malloc(sizeof(Chunk));
        c->title = strdup(line);
        c->body = NULL;
        c->body_count = 0;
        c->next = head;
        head = c;
        while (fgets(line, sizeof(line), f)) {
            line[strcspn(line, "\n")] = 0;
            if (strcmp(line, "--") == 0) {
                break;
            }
            c->body = realloc(c->body, (c->body_count + 1) * sizeof(char *));
            c->body[c->body_count++] = strdup(line);
        }
    }
    fclose(f);
}

void find_chunk(const char *title) {
    for (Chunk *c = head; c; c = c->next) {
        if (strcmp(c->title, title) == 0) {
            printf("%s\n", c->title);
            for (int i = 0; i < c->body_count; i++) {
                printf("%s\n", c->body[i]);
            }
            return;
        }
    }
}

void delete_chunk(const char *title) {
    Chunk *p = head;
    Chunk *prev = NULL;
    while (p) {
        if (strcmp(p->title, title) == 0) {
            Chunk *next = p->next;
            free(p->title);
            for (int i = 0; i < p->body_count; i++) {
                free(p->body[i]);
            }
            free(p->body);
            free(p);
            if (prev) {
                prev->next = next;
            } else {
                head = next;
            }
            return;
        }
        prev = p;
        p = p->next;
    }
}

void add_chunk() {
    char line[1000];
    if (!fgets(line, sizeof(line), stdin)) {
        return;
    }
    line[strcspn(line, "\n")] = 0;
    Chunk *c = malloc(sizeof(Chunk));
    c->title = strdup(line);
    c->body = NULL;
    c->body_count = 0;
    c->next = head;
    head = c;
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "--") == 0) {
            break;
        }
        c->body = realloc(c->body, (c->body_count + 1) * sizeof(char *));
        c->body[c->body_count++] = strdup(line);
    }
}

void print_chunks() {
    for (Chunk *c = head; c; c = c->next) {
        printf("%s\n", c->title);
        for (int i = 0; i < c->body_count; i++) {
            printf("%s\n", c->body[i]);
        }
        printf("--\n");
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char line[1000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char title[800];
        title[0] = 0;
        sscanf(line, "%s %[^\n]", cmd, title);
        if (strcmp(cmd, "FIND") == 0) {
            find_chunk(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_chunk(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            add_chunk();
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_chunks();
        }
    }
    while (head) {
        Chunk *n = head->next;
        free(head->title);
        for (int i = 0; i < head->body_count; i++) {
            free(head->body[i]);
        }
        free(head->body);
        free(head);
        head = n;
    }
    return 0;
}