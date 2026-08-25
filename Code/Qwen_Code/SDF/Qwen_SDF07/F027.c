// F027.c
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

typedef struct { int pri; char *title; char *note; size_t ord; } Rem;
Rem *rems = NULL;
size_t rc = 0, rcap = 0;
size_t order_cnt = 0;

void add_rem(int pri, const char *title, const char *note) {
    if (rc == rcap) {
        size_t nc = rcap ? rcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Rem)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Rem *tmp = realloc(rems, nc * sizeof(Rem));
        if (!tmp) { perror("realloc"); exit(1); }
        rems = tmp;
        rcap = nc;
    }
    rems[rc].pri = pri;
    rems[rc].title = strdup(title);
    rems[rc].note = strdup(note);
    rems[rc].ord = order_cnt++;
    if (!rems[rc].title || !rems[rc].note) { perror("strdup"); exit(1); }
    rc++;
}

void bump_rem(const char *title, int delta) {
    for (size_t i = 0; i < rc; i++) {
        if (strcmp(rems[i].title, title) == 0) {
            rems[i].pri += delta;
            return;
        }
    }
}

void rem_rem(const char *title) {
    for (size_t i = 0; i < rc; i++) {
        if (strcmp(rems[i].title, title) == 0) {
            free(rems[i].title); free(rems[i].note);
            rems[i] = rems[--rc];
            return;
        }
    }
}

int cmp_pri(const void *a, const void *b) {
    const Rem *ra = a, *rb = b;
    if (rb->pri != ra->pri) return rb->pri - ra->pri;
    return (ra->ord > rb->ord) - (ra->ord < rb->ord);
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            int pri; char title[256];
            if (sscanf(line, "%*s %d %255s", &pri, title) == 2) {
                char *note = strchr(line, ' ');
                if (note) { note = strchr(note + 1, ' '); if (note) note = strchr(note + 1, ' '); if (note) note++; }
                if (!note) note = "";
                add_rem(pri, title, note);
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char title[256]; int delta;
            if (sscanf(line, "%*s %255s %d", title, &delta) == 2) bump_rem(title, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256];
            if (sscanf(line, "%*s %255s", title) == 1) rem_rem(title);
        } else if (strcmp(cmd, "TOP") == 0) {
            int n;
            if (sscanf(line, "%*s %d", &n) == 1) {
                Rem *sorted = malloc(rc * sizeof(Rem));
                if (!sorted) { perror("malloc"); exit(1); }
                memcpy(sorted, rems, rc * sizeof(Rem));
                qsort(sorted, rc, sizeof(Rem), cmp_pri);
                size_t lim = (size_t)n < rc ? (size_t)n : rc;
                for (size_t i = 0; i < lim; i++) {
                    printf("%d %s %s\n", sorted[i].pri, sorted[i].title, sorted[i].note);
                }
                free(sorted);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < rc; i++) {
                printf("%d %s %s\n", rems[i].pri, rems[i].title, rems[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < rc; i++) { free(rems[i].title); free(rems[i].note); }
    free(rems);
    return 0;
}