// F006.c
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

typedef struct { char *item; int count; } Inv;
Inv *inv = NULL;
size_t ic = 0, icap = 0;

void add_inv(const char *item, int count) {
    for (size_t i = 0; i < ic; i++) {
        if (strcmp(inv[i].item, item) == 0) {
            inv[i].count += count;
            return;
        }
    }
    if (ic == icap) {
        size_t nc = icap ? icap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Inv)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Inv *tmp = realloc(inv, nc * sizeof(Inv));
        if (!tmp) { perror("realloc"); exit(1); }
        inv = tmp;
        icap = nc;
    }
    inv[ic].item = strdup(item);
    if (!inv[ic].item) { perror("strdup"); exit(1); }
    inv[ic].count = count;
    ic++;
}

void take_inv(const char *item, int count) {
    for (size_t i = 0; i < ic; i++) {
        if (strcmp(inv[i].item, item) == 0) {
            inv[i].count -= count;
            if (inv[i].count < 0) inv[i].count = 0;
            return;
        }
    }
}

void remove_inv(const char *item) {
    for (size_t i = 0; i < ic; i++) {
        if (strcmp(inv[i].item, item) == 0) {
            free(inv[i].item);
            inv[i] = inv[--ic];
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
            char item[256]; int count;
            if (sscanf(line, "%*s %255s %d", item, &count) == 2) add_inv(item, count);
        } else if (strcmp(cmd, "TAKE") == 0) {
            char item[256]; int count;
            if (sscanf(line, "%*s %255s %d", item, &count) == 2) take_inv(item, count);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char item[256];
            if (sscanf(line, "%*s %255s", item) == 1) remove_inv(item);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char item[256];
            if (sscanf(line, "%*s %255s", item) == 1) {
                int found = 0;
                for (size_t i = 0; i < ic; i++) {
                    if (strcmp(inv[i].item, item) == 0) { printf("%d\n", inv[i].count); found = 1; break; }
                }
                if (!found) printf("0\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < ic; i++) printf("%s %d\n", inv[i].item, inv[i].count);
        }
        free(line);
    }
    for (size_t i = 0; i < ic; i++) free(inv[i].item);
    free(inv);
    return 0;
}