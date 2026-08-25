// F017.c
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

typedef struct { char *key; char *val; } KV;
KV *kvs = NULL;
size_t kc = 0, kcap = 0;

void set_kv(const char *key, const char *val) {
    for (size_t i = 0; i < kc; i++) {
        if (strcmp(kvs[i].key, key) == 0) {
            char *tmp = strdup(val);
            if (!tmp) { perror("strdup"); exit(1); }
            free(kvs[i].val);
            kvs[i].val = tmp;
            return;
        }
    }
    if (kc == kcap) {
        size_t nc = kcap ? kcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(KV)) { fprintf(stderr, "Overflow\n"); exit(1); }
        KV *tmp = realloc(kvs, nc * sizeof(KV));
        if (!tmp) { perror("realloc"); exit(1); }
        kvs = tmp;
        kcap = nc;
    }
    kvs[kc].key = strdup(key);
    kvs[kc].val = strdup(val);
    if (!kvs[kc].key || !kvs[kc].val) { perror("strdup"); exit(1); }
    kc++;
}

void unset_kv(const char *key) {
    for (size_t i = 0; i < kc; i++) {
        if (strcmp(kvs[i].key, key) == 0) {
            free(kvs[i].key); free(kvs[i].val);
            kvs[i] = kvs[--kc];
            return;
        }
    }
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line;
    while ((line = read_line_file(f)) != NULL) {
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            set_kv(line, eq + 1);
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
        if (strcmp(cmd, "SET") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) != 1) { free(line); continue; }
            char *val = strchr(line, ' ');
            if (val) { val = strchr(val + 1, ' '); if (val) val++; }
            if (!val) val = "";
            set_kv(key, val);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) == 1) unset_kv(key);
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) == 1) {
                for (size_t i = 0; i < kc; i++) {
                    if (strcmp(kvs[i].key, key) == 0) { printf("%s\n", kvs[i].val); break; }
                }
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (size_t i = 0; i < kc; i++) printf("%s=%s\n", kvs[i].key, kvs[i].val);
        }
        free(line);
    }
    for (size_t i = 0; i < kc; i++) { free(kvs[i].key); free(kvs[i].val); }
    free(kvs);
    return 0;
}