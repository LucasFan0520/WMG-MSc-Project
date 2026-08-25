// F029.c
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

typedef struct { char *key; char *val; } Rec;
Rec *recs = NULL;
size_t rc = 0, rcap = 0;

void add_rec(const char *key, const char *val) {
    if (rc == rcap) {
        size_t nc = rcap ? rcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Rec)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Rec *tmp = realloc(recs, nc * sizeof(Rec));
        if (!tmp) { perror("realloc"); exit(1); }
        recs = tmp;
        rcap = nc;
    }
    recs[rc].key = strdup(key);
    recs[rc].val = strdup(val);
    if (!recs[rc].key || !recs[rc].val) { perror("strdup"); exit(1); }
    rc++;
}

void dedup() {
    for (size_t i = 0; i < rc; ) {
        int dup = 0;
        for (size_t j = 0; j < i; j++) {
            if (strcmp(recs[i].key, recs[j].key) == 0) { dup = 1; break; }
        }
        if (dup) {
            free(recs[i].key); free(recs[i].val);
            recs[i] = recs[--rc];
        } else i++;
    }
}

void del_key(const char *key) {
    for (size_t i = 0; i < rc; ) {
        if (strcmp(recs[i].key, key) == 0) {
            free(recs[i].key); free(recs[i].val);
            recs[i] = recs[--rc];
        } else i++;
    }
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            char key[256], val[256];
            if (sscanf(line, "%*s %255s %255s", key, val) == 2) add_rec(key, val);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup();
        } else if (strcmp(cmd, "DELETE") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) == 1) del_key(key);
        } else if (strcmp(cmd, "FIND") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) == 1) {
                for (size_t i = 0; i < rc; i++) {
                    if (strcmp(recs[i].key, key) == 0) printf("%s\n", recs[i].val);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < rc; i++) printf("%s %s\n", recs[i].key, recs[i].val);
        }
        free(line);
    }
    for (size_t i = 0; i < rc; i++) { free(recs[i].key); free(recs[i].val); }
    free(recs);
    return 0;
}