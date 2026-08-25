// F039.c
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

typedef struct { char *name; size_t rows; size_t cols; char ***cells; } Tab;
Tab *tabs = NULL;
size_t tc = 0, tcap = 0;

Tab* find_tab(const char *name) {
    for (size_t i = 0; i < tc; i++) if (strcmp(tabs[i].name, name) == 0) return &tabs[i];
    return NULL;
}

void add_tab(const char *name, size_t rows, size_t cols) {
    if (find_tab(name)) return;
    if (tc == tcap) {
        size_t nc = tcap ? tcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Tab)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Tab *tmp = realloc(tabs, nc * sizeof(Tab));
        if (!tmp) { perror("realloc"); exit(1); }
        tabs = tmp;
        tcap = nc;
    }
    tabs[tc].name = strdup(name);
    tabs[tc].rows = rows;
    tabs[tc].cols = cols;
    tabs[tc].cells = malloc(rows * sizeof(char**));
    if (!tabs[tc].cells) { perror("malloc"); exit(1); }
    for (size_t i = 0; i < rows; i++) {
        tabs[tc].cells[i] = calloc(cols, sizeof(char*));
        if (!tabs[tc].cells[i]) { perror("calloc"); exit(1); }
    }
    if (!tabs[tc].name) { perror("strdup"); exit(1); }
    tc++;
}

void del_tab(const char *name) {
    for (size_t i = 0; i < tc; i++) {
        if (strcmp(tabs[i].name, name) == 0) {
            free(tabs[i].name);
            for (size_t r = 0; r < tabs[i].rows; r++) {
                for (size_t c = 0; c < tabs[i].cols; c++) free(tabs[i].cells[r][c]);
                free(tabs[i].cells[r]);
            }
            free(tabs[i].cells);
            tabs[i] = tabs[--tc];
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
        if (strcmp(cmd, "TABLE") == 0) {
            char name[256]; size_t rows, cols;
            if (sscanf(line, "%*s %255s %zu %zu", name, &rows, &cols) == 3) add_tab(name, rows, cols);
        } else if (strcmp(cmd, "SET") == 0) {
            char name[256], val[256]; size_t r, c;
            if (sscanf(line, "%*s %255s %zu %zu %255s", name, &r, &c, val) == 4) {
                Tab *t = find_tab(name);
                if (t && r < t->rows && c < t->cols) {
                    free(t->cells[r][c]);
                    t->cells[r][c] = strdup(val);
                    if (!t->cells[r][c]) { perror("strdup"); exit(1); }
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char name[256]; size_t r, c;
            if (sscanf(line, "%*s %255s %zu %zu", name, &r, &c) == 3) {
                Tab *t = find_tab(name);
                if (t && r < t->rows && c < t->cols) {
                    printf("%s\n", t->cells[r][c] ? t->cells[r][c] : "EMPTY");
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) del_tab(name);
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (size_t i = 0; i < tc; i++) printf("%s\n", tabs[i].name);
        }
        free(line);
    }
    for (size_t i = 0; i < tc; i++) {
        free(tabs[i].name);
        for (size_t r = 0; r < tabs[i].rows; r++) {
            for (size_t c = 0; c < tabs[i].cols; c++) free(tabs[i].cells[r][c]);
            free(tabs[i].cells[r]);
        }
        free(tabs[i].cells);
    }
    free(tabs);
    return 0;
}