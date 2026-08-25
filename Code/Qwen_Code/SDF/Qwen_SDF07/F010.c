// F010.c
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

typedef struct { char *key; char **vals; size_t vc; size_t vcap; } Idx;
Idx *idxs = NULL;
size_t ic = 0, icap = 0;

Idx* find_idx(const char *key) {
    for (size_t i = 0; i < ic; i++) if (strcmp(idxs[i].key, key) == 0) return &idxs[i];
    return NULL;
}

Idx* add_idx(const char *key) {
    if (ic == icap) {
        size_t nc = icap ? icap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Idx)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Idx *tmp = realloc(idxs, nc * sizeof(Idx));
        if (!tmp) { perror("realloc"); exit(1); }
        idxs = tmp;
        icap = nc;
    }
    idxs[ic].key = strdup(key);
    idxs[ic].vals = NULL;
    idxs[ic].vc = 0;
    idxs[ic].vcap = 0;
    if (!idxs[ic].key) { perror("strdup"); exit(1); }
    return &idxs[ic++];
}

void add_val(Idx *idx, const char *val) {
    if (idx->vc == idx->vcap) {
        size_t nc = idx->vcap ? idx->vcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(char*)) { fprintf(stderr, "Overflow\n"); exit(1); }
        char **tmp = realloc(idx->vals, nc * sizeof(char*));
        if (!tmp) { perror("realloc"); exit(1); }
        idx->vals = tmp;
        idx->vcap = nc;
    }
    idx->vals[idx->vc] = strdup(val);
    if (!idx->vals[idx->vc]) { perror("strdup"); exit(1); }
    idx->vc++;
}

void rem_val(Idx *idx, const char *val) {
    for (size_t i = 0; i < idx->vc; i++) {
        if (strcmp(idx->vals[i], val) == 0) {
            free(idx->vals[i]);
            idx->vals[i] = idx->vals[--idx->vc];
            return;
        }
    }
}

void erase_idx(const char *key) {
    for (size_t i = 0; i < ic; i++) {
        if (strcmp(idxs[i].key, key) == 0) {
            free(idxs[i].key);
            for (size_t j = 0; j < idxs[i].vc; j++) free(idxs[i].vals[j]);
            free(idxs[i].vals);
            idxs[i] = idxs[--ic];
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
            char key[256], val[256];
            if (sscanf(line, "%*s %255s %255s", key, val) == 2) {
                Idx *idx = find_idx(key);
                if (!idx) idx = add_idx(key);
                add_val(idx, val);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char key[256], val[256];
            if (sscanf(line, "%*s %255s %255s", key, val) == 2) {
                Idx *idx = find_idx(key);
                if (idx) rem_val(idx, val);
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) == 1) erase_idx(key);
        } else if (strcmp(cmd, "VALUES") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) == 1) {
                Idx *idx = find_idx(key);
                if (idx) for (size_t i = 0; i < idx->vc; i++) printf("%s\n", idx->vals[i]);
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (size_t i = 0; i < ic; i++) printf("%s\n", idxs[i].key);
        }
        free(line);
    }
    for (size_t i = 0; i < ic; i++) {
        free(idxs[i].key);
        for (size_t j = 0; j < idxs[i].vc; j++) free(idxs[i].vals[j]);
        free(idxs[i].vals);
    }
    free(idxs);
    return 0;
}