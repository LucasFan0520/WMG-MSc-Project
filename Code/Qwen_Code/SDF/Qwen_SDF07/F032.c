// F032.c
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

typedef struct { char *key; char *val; } KV;
KV *kvs = NULL;
size_t kc = 0, kcap = 0;

KV *stage = NULL;
size_t sc = 0, scap = 0;
int in_trans = 0;

void set_kv(KV *arr, size_t *c, size_t *cap, const char *key, const char *val) {
    for (size_t i = 0; i < *c; i++) {
        if (strcmp(arr[i].key, key) == 0) {
            char *tmp = strdup(val);
            if (!tmp) { perror("strdup"); exit(1); }
            free(arr[i].val);
            arr[i].val = tmp;
            return;
        }
    }
    if (*c == *cap) {
        size_t nc = *cap ? *cap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(KV)) { fprintf(stderr, "Overflow\n"); exit(1); }
        KV *tmp = realloc(arr, nc * sizeof(KV));
        if (!tmp) { perror("realloc"); exit(1); }
        arr = tmp;
        *cap = nc;
    }
    arr[*c].key = strdup(key);
    arr[*c].val = strdup(val);
    if (!arr[*c].key || !arr[*c].val) { perror("strdup"); exit(1); }
    (*c)++;
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "SET") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) != 1) { free(line); continue; }
            char *val = strchr(line, ' ');
            if (val) { val = strchr(val + 1, ' '); if (val) val++; }
            if (!val) val = "";
            set_kv(kvs, &kc, &kcap, key, val);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_trans = 1;
            sc = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_trans) {
                char key[256];
                if (sscanf(line, "%*s %255s", key) != 1) { free(line); continue; }
                char *val = strchr(line, ' ');
                if (val) { val = strchr(val + 1, ' '); if (val) val++; }
                if (!val) val = "";
                set_kv(stage, &sc, &scap, key, val);
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_trans) {
                for (size_t i = 0; i < sc; i++) {
                    set_kv(kvs, &kc, &kcap, stage[i].key, stage[i].val);
                }
                for (size_t i = 0; i < sc; i++) { free(stage[i].key); free(stage[i].val); }
                sc = 0;
                in_trans = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_trans) {
                for (size_t i = 0; i < sc; i++) { free(stage[i].key); free(stage[i].val); }
                sc = 0;
                in_trans = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) == 1) {
                for (size_t i = 0; i < kc; i++) {
                    if (strcmp(kvs[i].key, key) == 0) { printf("%s\n", kvs[i].val); break; }
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (size_t i = 0; i < kc; i++) printf("%s=%s\n", kvs[i].key, kvs[i].val);
        }
        free(line);
    }
    for (size_t i = 0; i < kc; i++) { free(kvs[i].key); free(kvs[i].val); }
    free(kvs);
    for (size_t i = 0; i < sc; i++) { free(stage[i].key); free(stage[i].val); }
    free(stage);
    return 0;
}