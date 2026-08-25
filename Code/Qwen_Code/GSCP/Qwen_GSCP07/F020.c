// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Chunk { char *title; char **body; int lines; struct Chunk *next; } Chunk;
Chunk *head = NULL;

void add_chunk(const char *title, char **body, int lines) {
    Chunk *c = malloc(sizeof(Chunk));
    c->title = strdup(title);
    c->lines = lines;
    c->body = malloc(lines * sizeof(char *));
    for (int i = 0; i < lines; i++) c->body[i] = strdup(body[i]);
    c->next = head;
    head = c;
}

void delete_chunk(const char *title) {
    Chunk **p = &head;
    while (*p) {
        if (strcmp((*p)->title, title) == 0) {
            Chunk *tmp = *p;
            *p = tmp->next;
            for (int i = 0; i < tmp->lines; i++) free(tmp->body[i]);
            free(tmp->body);
            free(tmp->title);
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

void find_chunk(const char *title) {
    for (Chunk *c = head; c; c = c->next) {
        if (strcmp(c->title, title) == 0) {
            printf("%s\n", c->title);
            for (int i = 0; i < c->lines; i++) printf("%s\n", c->body[i]);
            return;
        }
    }
}

void print_chunks() {
    for (Chunk *c = head; c; c = c->next) {
        printf("%s\n", c->title);
        for (int i = 0; i < c->lines; i++) printf("%s\n", c->body[i]);
        printf("--\n");
    }
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1024];
    char title[256] = "";
    char **body = NULL;
    int lines = 0, cap = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "--") == 0) {
            if (strlen(title) > 0) {
                add_chunk(title, body, lines);
                for (int i = 0; i < lines; i++) free(body[i]);
                free(body);
                body = NULL;
                lines = 0;
                cap = 0;
                title[0] = 0;
            }
        } else {
            if (title[0] == 0) {
                strcpy(title, line);
            } else {
                if (lines == cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    body = realloc(body, cap * sizeof(char *));
                }
                body[lines++] = strdup(line);
            }
        }
    }
    if (strlen(title) > 0) {
        add_chunk(title, body, lines);
        for (int i = 0; i < lines; i++) free(body[i]);
        free(body);
    }
    fclose(f);
}

void cleanup() {
    while (head) {
        Chunk *tmp = head->next;
        for (int i = 0; i < head->lines; i++) free(head->body[i]);
        free(head->body);
        free(head->title);
        free(head);
        head = tmp;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], title[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) find_chunk(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) delete_chunk(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) {
                char **body = NULL;
                int lines = 0, cap = 0;
                while (fgets(line, sizeof(line), stdin)) {
                    line[strcspn(line, "\n")] = 0;
                    if (strcmp(line, "--") == 0) break;
                    if (lines == cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        body = realloc(body, cap * sizeof(char *));
                    }
                    body[lines++] = strdup(line);
                }
                add_chunk(title, body, lines);
                for (int i = 0; i < lines; i++) free(body[i]);
                free(body);
            }
        } else if (strcmp(cmd, "PRINT") == 0) print_chunks();
    }
    cleanup();
    return 0;
}