// F021.c
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

typedef struct { char *ing; char *note; } Ing;
typedef struct { char *name; Ing *ings; size_t ic; size_t icap; } Recipe;
Recipe *recs = NULL;
size_t rc = 0, rcap = 0;

Recipe* find_rec(const char *name) {
    for (size_t i = 0; i < rc; i++) if (strcmp(recs[i].name, name) == 0) return &recs[i];
    return NULL;
}

void add_rec(const char *name) {
    if (find_rec(name)) return;
    if (rc == rcap) {
        size_t nc = rcap ? rcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Recipe)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Recipe *tmp = realloc(recs, nc * sizeof(Recipe));
        if (!tmp) { perror("realloc"); exit(1); }
        recs = tmp;
        rcap = nc;
    }
    recs[rc].name = strdup(name);
    recs[rc].ings = NULL;
    recs[rc].ic = 0;
    recs[rc].icap = 0;
    if (!recs[rc].name) { perror("strdup"); exit(1); }
    rc++;
}

void add_ing(Recipe *r, const char *ing, const char *note) {
    if (r->ic == r->icap) {
        size_t nc = r->icap ? r->icap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Ing)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Ing *tmp = realloc(r->ings, nc * sizeof(Ing));
        if (!tmp) { perror("realloc"); exit(1); }
        r->ings = tmp;
        r->icap = nc;
    }
    r->ings[r->ic].ing = strdup(ing);
    r->ings[r->ic].note = strdup(note);
    if (!r->ings[r->ic].ing || !r->ings[r->ic].note) { perror("strdup"); exit(1); }
    r->ic++;
}

void rem_ing(Recipe *r, const char *ing) {
    for (size_t i = 0; i < r->ic; i++) {
        if (strcmp(r->ings[i].ing, ing) == 0) {
            free(r->ings[i].ing); free(r->ings[i].note);
            r->ings[i] = r->ings[--r->ic];
            return;
        }
    }
}

void del_rec(const char *name) {
    for (size_t i = 0; i < rc; i++) {
        if (strcmp(recs[i].name, name) == 0) {
            free(recs[i].name);
            for (size_t j = 0; j < recs[i].ic; j++) { free(recs[i].ings[j].ing); free(recs[i].ings[j].note); }
            free(recs[i].ings);
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
        if (strcmp(cmd, "RECIPE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) add_rec(name);
        } else if (strcmp(cmd, "ING") == 0) {
            char name[256], ing[256];
            if (sscanf(line, "%*s %255s %255s", name, ing) == 2) {
                Recipe *r = find_rec(name);
                if (r) {
                    char *note = strchr(line, ' ');
                    if (note) { note = strchr(note + 1, ' '); if (note) note = strchr(note + 1, ' '); if (note) note++; }
                    if (!note) note = "";
                    add_ing(r, ing, note);
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char name[256], ing[256];
            if (sscanf(line, "%*s %255s %255s", name, ing) == 2) {
                Recipe *r = find_rec(name);
                if (r) rem_ing(r, ing);
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) del_rec(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                Recipe *r = find_rec(name);
                if (r) {
                    printf("%s\n", r->name);
                    for (size_t i = 0; i < r->ic; i++) {
                        printf("%s ", r->ings[i].ing);
                        for (size_t j = 0; r->ings[i].note[j]; j++) putchar(r->ings[i].note[j] == ' ' ? '_' : r->ings[i].note[j]);
                        printf("\n");
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < rc; i++) printf("%s\n", recs[i].name);
        }
        free(line);
    }
    for (size_t i = 0; i < rc; i++) {
        free(recs[i].name);
        for (size_t j = 0; j < recs[i].ic; j++) { free(recs[i].ings[j].ing); free(recs[i].ings[j].note); }
        free(recs[i].ings);
    }
    free(recs);
    return 0;
}