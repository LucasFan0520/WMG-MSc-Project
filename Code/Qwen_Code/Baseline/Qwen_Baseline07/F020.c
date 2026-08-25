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
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        size_t l = strlen(line);
        if (l > 0 && line[l-1] == '\n') line[l-1] = '\0';
        if (strcmp(line, "--") == 0) continue;
        Chunk *c = malloc(sizeof(Chunk));
        c->title = strdup(line);
        c->body = NULL; c->body_count = 0; c->next = head; head = c;
        while (fgets(line, sizeof(line), f)) {
            l = strlen(line);
            if (l > 0 && line[l-1] == '\n') line[l-1] = '\0';
            if (strcmp(line, "--") == 0) break;
            c->body = realloc(c->body, (c->body_count + 1) * sizeof(char*));
            c->body[c->body_count++] = strdup(line);
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char cmd[20], title[1024];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "FIND") == 0) {
            fgets(title, sizeof(title), stdin);
            if (title[0] == ' ') memmove(title, title + 1, strlen(title));
            size_t l = strlen(title);
            if (l > 0 && title[l-1] == '\n') title[l-1] = '\0';
            for (Chunk *c = head; c; c = c->next)
                if (strcmp(c->title, title) == 0) {
                    printf("%s\n", c->title);
                    for (int i = 0; i < c->body_count; i++) printf("%s\n", c->body[i]);
                    break;
                }
        } else if (strcmp(cmd, "DELETE") == 0) {
            fgets(title, sizeof(title), stdin);
            if (title[0] == ' ') memmove(title, title + 1, strlen(title));
            size_t l = strlen(title);
            if (l > 0 && title[l-1] == '\n') title[l-1] = '\0';
            Chunk *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->title, title) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->title);
                    for (int i = 0; i < curr->body_count; i++) free(curr->body[i]);
                    free(curr->body); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            fgets(title, sizeof(title), stdin);
            if (title[0] == ' ') memmove(title, title + 1, strlen(title));
            size_t l = strlen(title);
            if (l > 0 && title[l-1] == '\n') title[l-1] = '\0';
            Chunk *c = malloc(sizeof(Chunk));
            c->title = strdup(title);
            c->body = NULL; c->body_count = 0; c->next = head; head = c;
            char line[1024];
            while (fgets(line, sizeof(line), stdin)) {
                l = strlen(line);
                if (l > 0 && line[l-1] == '\n') line[l-1] = '\0';
                if (strcmp(line, "--") == 0) break;
                c->body = realloc(c->body, (c->body_count + 1) * sizeof(char*));
                c->body[c->body_count++] = strdup(line);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Chunk *c = head; c; c = c->next) {
                printf("%s\n", c->title);
                for (int i = 0; i < c->body_count; i++) printf("%s\n", c->body[i]);
                printf("--\n");
            }
        }
    }
    return 0;
}