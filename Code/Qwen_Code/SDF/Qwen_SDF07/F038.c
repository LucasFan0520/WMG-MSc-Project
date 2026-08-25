// F038.c
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

typedef struct { char *name; char *enc; } RLE;
RLE *rles = NULL;
size_t rc = 0, rcap = 0;

void store_rle(const char *name, const char *enc) {
    if (rc == rcap) {
        size_t nc = rcap ? rcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(RLE)) { fprintf(stderr, "Overflow\n"); exit(1); }
        RLE *tmp = realloc(rles, nc * sizeof(RLE));
        if (!tmp) { perror("realloc"); exit(1); }
        rles = tmp;
        rcap = nc;
    }
    rles[rc].name = strdup(name);
    rles[rc].enc = strdup(enc);
    if (!rles[rc].name || !rles[rc].enc) { perror("strdup"); exit(1); }
    rc++;
}

void del_rle(const char *name) {
    for (size_t i = 0; i < rc; i++) {
        if (strcmp(rles[i].name, name) == 0) {
            free(rles[i].name); free(rles[i].enc);
            rles[i] = rles[--rc];
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
        if (strcmp(cmd, "STORE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) != 1) { free(line); continue; }
            char *enc = strchr(line, ' ');
            if (enc) { enc = strchr(enc + 1, ' '); if (enc) enc++; }
            if (enc) store_rle(name, enc);
        } else if (strcmp(cmd, "DECODE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                for (size_t i = 0; i < rc; i++) {
                    if (strcmp(rles[i].name, name) == 0) {
                        char *p = rles[i].enc;
                        while (*p) {
                            unsigned long count = 0;
                            while (*p >= '0' && *p <= '9') {
                                count = count * 10 + (*p - '0');
                                if (count > 1000000) { count = 0; break; }
                                p++;
                            }
                            if (count > 0 && *p) {
                                for (unsigned long j = 0; j < count; j++) {
                                    putchar(*p == ' ' ? '_' : *p);
                                }
                                p++;
                            } else {
                                break;
                            }
                        }
                        printf("\n");
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) del_rle(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < rc; i++) printf("%s\n", rles[i].name);
        }
        free(line);
    }
    for (size_t i = 0; i < rc; i++) { free(rles[i].name); free(rles[i].enc); }
    free(rles);
    return 0;
}