// F031.c
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

typedef struct { char *name; char *note; } Rec;
Rec *recs = NULL;
size_t rc = 0, rcap = 0;

void add_rec(const char *name, const char *note) {
    if (rc == rcap) {
        size_t nc = rcap ? rcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Rec)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Rec *tmp = realloc(recs, nc * sizeof(Rec));
        if (!tmp) { perror("realloc"); exit(1); }
        recs = tmp;
        rcap = nc;
    }
    recs[rc].name = strdup(name);
    recs[rc].note = strdup(note);
    if (!recs[rc].name || !recs[rc].note) { perror("strdup"); exit(1); }
    rc++;
}

void del_rec(const char *name) {
    for (size_t i = 0; i < rc; i++) {
        if (strcmp(recs[i].name, name) == 0) {
            free(recs[i].name); free(recs[i].note);
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
        if (strcmp(cmd, "BATCH") == 0) {
            int n;
            if (sscanf(line, "%*s %d", &n) == 1 && n >= 0) {
                Rec *batch = malloc(n * sizeof(Rec));
                if (!batch && n > 0) { perror("malloc"); exit(1); }
                int valid = 1;
                for (int i = 0; i < n; i++) {
                    char *bline = read_line();
                    if (!bline) { valid = 0; break; }
                    char name[256];
                    if (sscanf(bline, "%255s", name) != 1) { valid = 0; free(bline); break; }
                    char *note = strchr(bline, ' ');
                    if (note) note++; else note = "";
                    batch[i].name = strdup(name);
                    batch[i].note = strdup(note);
                    if (!batch[i].name || !batch[i].note) { valid = 0; free(bline); break; }
                    free(bline);
                }
                if (valid) {
                    for (int i = 0; i < n; i++) {
                        add_rec(batch[i].name, batch[i].note);
                        free(batch[i].name); free(batch[i].note);
                    }
                } else {
                    for (int i = 0; i < n; i++) {
                        if (batch[i].name) free(batch[i].name);
                        if (batch[i].note) free(batch[i].note);
                    }
                }
                free(batch);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) del_rec(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                for (size_t i = 0; i < rc; i++) {
                    if (strcmp(recs[i].name, name) == 0) {
                        printf("%s %s\n", recs[i].name, recs[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < rc; i++) printf("%s %s\n", recs[i].name, recs[i].note);
        }
        free(line);
    }
    for (size_t i = 0; i < rc; i++) { free(recs[i].name); free(recs[i].note); }
    free(recs);
    return 0;
}