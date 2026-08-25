// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Cell { char *val; } Cell;
typedef struct Table { char *name; int rows; int cols; Cell *cells; struct Table *next; } Table;
Table *tables = NULL;

Table *find_table(const char *name) {
    for (Table *t = tables; t; t = t->next)
        if (strcmp(t->name, name) == 0) return t;
    return NULL;
}

void table_create(const char *name, int rows, int cols) {
    if (find_table(name) || rows <= 0 || cols <= 0) return;
    Table *t = malloc(sizeof(Table));
    if (!t) return;
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = calloc(rows * cols, sizeof(Cell));
    if (!t->cells) { free(t->name); free(t); return; }
    t->next = tables;
    tables = t;
}

void table_set(const char *name, int row, int col, const char *val) {
    Table *t = find_table(name);
    if (!t || row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    Cell *c = &t->cells[row * t->cols + col];
    free(c->val);
    c->val = strdup(val);
}

void table_get(const char *name, int row, int col) {
    Table *t = find_table(name);
    if (!t || row < 0 || row >= t->rows || col < 0 || col >= t->cols) { printf("EMPTY\n"); return; }
    Cell *c = &t->cells[row * t->cols + col];
    if (c->val) printf("%s\n", c->val);
    else printf("EMPTY\n");
}

void table_delete(const char *name) {
    Table *prev = NULL;
    for (Table *t = tables; t; prev = t, t = t->next) {
        if (strcmp(t->name, name) == 0) {
            if (prev) prev->next = t->next; else tables = t->next;
            free(t->name);
            for (int i = 0; i < t->rows * t->cols; i++) free(t->cells[i].val);
            free(t->cells);
            free(t);
            return;
        }
    }
}

void list_tables(void) {
    for (Table *t = tables; t; t = t->next)
        printf("%s\n", t->name);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "TABLE") == 0) {
            char name[256]; int rows, cols;
            if (sscanf(rest, "%255s %d %d", name, &rows, &cols) == 3)
                table_create(name, rows, cols);
        } else if (strcmp(cmd, "SET") == 0) {
            char name[256]; int row, col; char val[4096];
            if (sscanf(rest, "%255s %d %d", name, &row, &col) == 3) {
                char *v = rest;
                while (*v == ' ') v++;
                while (*v && *v != ' ') v++;
                while (*v == ' ') v++;
                while (*v && *v != ' ') v++;
                while (*v == ' ') v++;
                while (*v && *v != ' ') v++;
                while (*v == ' ') v++;
                strcpy(val, v);
                table_set(name, row, col, val);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char name[256]; int row, col;
            if (sscanf(rest, "%255s %d %d", name, &row, &col) == 3)
                table_get(name, row, col);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) table_delete(name);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    while (tables) { Table *t = tables; tables = t->next; free(t->name); for (int i = 0; i < t->rows * t->cols; i++) free(t->cells[i].val); free(t->cells); free(t); }
    return 0;
}