// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line_file(FILE *f) {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct { char *title; char **body; size_t bc; size_t bcap; } Chunk;
Chunk *chunks = NULL;
size_t cc = 0, ccap = 0;

void add_chunk(const char *title) {
    if (cc == ccap) {
        size_t nc = ccap ? ccap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Chunk)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Chunk *tmp = realloc(chunks, nc * sizeof(Chunk));
        if (!tmp) { perror("realloc"); exit(1); }
        chunks = tmp;
        ccap = nc;
    }
    chunks[cc].title = strdup(title);
    chunks[cc].body = NULL;
    chunks[cc].bc = 0;
    chunks[cc].bcap = 0;
    if (!chunks[cc].title) { perror("strdup"); exit(1); }
    cc++;
}

void add_body(Chunk *c, const char *line) {
    if (c->bc == c->bcap) {
        size_t nc = c->bcap ? c->bcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(char*)) { fprintf(stderr, "Overflow\n"); exit(1); }
        char **tmp = realloc(c->body, nc * sizeof(char*));
        if (!tmp) { perror("realloc"); exit(1); }
        c->body = tmp;
        c->bcap = nc;
    }
    c->body[c->bc] = strdup(line);
    if (!c->body[c->bc]) { perror("strdup"); exit(1); }
    c->bc++;
}

void del_chunk(const char *title) {
    for (size_t i = 0; i < cc; i++) {
        if (strcmp(chunks[i].title, title) == 0) {
            free(chunks[i].title);
            for (size_t j = 0; j < chunks[i].bc; j++) free(chunks[i].body[j]);
            free(chunks[i].body);
            chunks[i] = chunks[--cc];
            return;
        }
    }
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line;
    int in_chunk = 0;
    Chunk *curr = NULL;
    while ((line = read_line_file(f)) != NULL) {
        if (strcmp(line, "--") == 0) {
            in_chunk = 0;
            curr = NULL;
        } else if (!in_chunk) {
            add_chunk(line);
            curr = &chunks[cc-1];
            in_chunk = 1;
        } else if (curr) {
            add_body(curr, line);
        }
        free(line);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) {
                for (size_t i = 0; i < cc; i++) {
                    if (strcmp(chunks[i].title, title) == 0) {
                        printf("%s\n", chunks[i].title);
                        for (size_t j = 0; j < chunks[i].bc; j++) printf("%s\n", chunks[i].body[j]);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) del_chunk(title);
        } else if (strcmp(cmd, "ADD") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) {
                add_chunk(title);
                Chunk *c = &chunks[cc-1];
                char *bline;
                while ((bline = read_line()) != NULL) {
                    if (strcmp(bline, "--") == 0) { free(bline); break; }
                    add_body(c, bline);
                    free(bline);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < cc; i++) {
                printf("%s\n", chunks[i].title);
                for (size_t j = 0; j < chunks[i].bc; j++) printf("%s\n", chunks[i].body[j]);
                printf("--\n");
            }
        }
        free(line);
    }
    for (size_t i = 0; i < cc; i++) {
        free(chunks[i].title);
        for (size_t j = 0; j < chunks[i].bc; j++) free(chunks[i].body[j]);
        free(chunks[i].body);
    }
    free(chunks);
    return 0;
}