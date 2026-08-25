// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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

typedef struct { char *name; char *data; } Rec;
Rec *recs = NULL;
size_t rc = 0, rcap = 0;

void add_rec(const char *name, const char *raw) {
    char *colon = strchr(raw, ':');
    if (!colon) return;
    *colon = '\0';
    size_t len = strlen(raw);
    for (size_t i = 0; i < len; i++) {
        if (raw[i] < '0' || raw[i] > '9') return;
    }
    size_t declen = (size_t)strtoull(raw, NULL, 10);
    char *data = colon + 1;
    if (strlen(data) != declen) return;
    
    if (rc == rcap) {
        size_t nc = rcap ? rcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Rec)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Rec *tmp = realloc(recs, nc * sizeof(Rec));
        if (!tmp) { perror("realloc"); exit(1); }
        recs = tmp;
        rcap = nc;
    }
    recs[rc].name = strdup(name);
    recs[rc].data = strdup(data);
    if (!recs[rc].name || !recs[rc].data) { perror("strdup"); exit(1); }
    rc++;
}

void del_rec(const char *name) {
    for (size_t i = 0; i < rc; i++) {
        if (strcmp(recs[i].name, name) == 0) {
            free(recs[i].name); free(recs[i].data);
            recs[i] = recs[--rc];
            return;
        }
    }
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) != 1) { free(line); continue; }
            char *raw = strchr(line, ' ');
            if (raw) { raw = strchr(raw + 1, ' '); if (raw) raw++; }
            if (raw) add_rec(name, raw);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) del_rec(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                for (size_t i = 0; i < rc; i++) {
                    if (strcmp(recs[i].name, name) == 0) {
                        for (size_t j = 0; recs[i].data[j]; j++) putchar(recs[i].data[j] == ' ' ? '_' : recs[i].data[j]);
                        printf("\n");
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < rc; i++) printf("%s\n", recs[i].name);
        }
        free(line);
    }
    for (size_t i = 0; i < rc; i++) { free(recs[i].name); free(recs[i].data); }
    free(recs);
    return 0;
}