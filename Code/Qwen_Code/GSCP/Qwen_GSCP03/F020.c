// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Chunk {
    char *title;
    char **body;
    int body_count, body_cap;
    struct Chunk *next;
} Chunk;

Chunk *head = NULL;

void free_chunk(Chunk *c) {
    for (int i = 0; i < c->body_count; i++) free(c->body[i]);
    free(c->body);
    free(c->title);
    free(c);
}

void free_list() {
    while (head) {
        Chunk *t = head; head = head->next;
        free_chunk(t);
    }
}

Chunk *find_chunk(const char *title) {
    for (Chunk *c = head; c; c = c->next)
        if (strcmp(c->title, title) == 0) return c;
    return NULL;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1024];
    Chunk *curr = NULL;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "--") == 0) {
            curr = NULL;
        } else if (!curr) {
            curr = malloc(sizeof(Chunk));
            curr->title = strdup(line);
            curr->body = NULL; curr->body_count = 0; curr->body_cap = 0;
            curr->next = head; head = curr;
        } else {
            if (curr->body_count == curr->body_cap) {
                curr->body_cap = curr->body_cap ? curr->body_cap * 2 : 4;
                curr->body = realloc(curr->body, curr->body_cap * sizeof(char*));
            }
            curr->body[curr->body_count++] = strdup(line);
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);

    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], title[256];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255[^\n]", title) != 1) continue;
            Chunk *c = find_chunk(title);
            if (c) {
                printf("%s\n", c->title);
                for (int i = 0; i < c->body_count; i++) printf("%s\n", c->body[i]);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255[^\n]", title) != 1) continue;
            Chunk *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->title, title) == 0) {
                    Chunk *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free_chunk(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255[^\n]", title) != 1) continue;
            if (find_chunk(title)) continue;
            Chunk *c = malloc(sizeof(Chunk));
            c->title = strdup(title);
            c->body = NULL; c->body_count = 0; c->body_cap = 0;
            c->next = head; head = c;
            
            while (fgets(line, sizeof(line), stdin)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, "--") == 0) break;
                if (c->body_count == c->body_cap) {
                    c->body_cap = c->body_cap ? c->body_cap * 2 : 4;
                    c->body = realloc(c->body, c->body_cap * sizeof(char*));
                }
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
    free_list();
    return 0;
}