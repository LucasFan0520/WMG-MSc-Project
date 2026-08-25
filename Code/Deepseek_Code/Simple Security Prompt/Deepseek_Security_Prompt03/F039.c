/*
 * F039.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct cell {
    char *value;
    struct cell *next;
} Cell;

typedef struct table {
    char *name;
    int rows;
    int cols;
    Cell **cells; // flat array of size rows*cols
    struct table *next;
} Table;

Table *tables = NULL;

Table *find_table(const char *name) {
    Table *t = tables;
    while (t) {
        if (strcmp(t->name, name) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void table_create(const char *name, int rows, int cols) {
    if (find_table(name)) return;
    if (rows <= 0 || cols <= 0) return;
    Table *t = malloc(sizeof(Table));
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = calloc(rows * cols, sizeof(Cell*));
    t->next = tables;
    tables = t;
}

void table_set(const char *name, int row, int col, const char *value) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    int idx = row * t->cols + col;
    if (t->cells[idx]) {
        free(t->cells[idx]->value);
        t->cells[idx]->value = strdup(value);
    } else {
        Cell *c = malloc(sizeof(Cell));
        c->value = strdup(value);
        c->next = NULL;
        t->cells[idx] = c;
    }
}

void table_get(const char *name, int row, int col) {
    Table *t = find_table(name);
    if (!t) {
        printf("EMPTY\n");
        return;
    }
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) {
        printf("EMPTY\n");
        return;
    }
    int idx = row * t->cols + col;
    if (t->cells[idx]) {
        printf("%s\n", t->cells[idx]->value);
    } else {
        printf("EMPTY\n");
    }
}

void table_delete(const char *name) {
    Table *prev = NULL, *t = tables;
    while (t) {
        if (strcmp(t->name, name) == 0) {
            if (prev) prev->next = t->next;
            else tables = t->next;
            free(t->name);
            for (int i = 0; i < t->rows * t->cols; i++) {
                if (t->cells[i]) {
                    free(t->cells[i]->value);
                    free(t->cells[i]);
                }
            }
            free(t->cells);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void table_tables(void) {
    Table *t = tables;
    while (t) {
        printf("%s\n", t->name);
        t = t->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256];
        if (sscanf(line, "%31s %255s", cmd, name) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(name) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "TABLE") == 0) {
            int rows, cols;
            if (sscanf(rest, "%d %d", &rows, &cols) == 2) {
                table_create(name, rows, cols);
            }
        } else if (strcmp(cmd, "SET") == 0) {
            int row, col;
            char value[4096];
            if (sscanf(rest, "%d %d", &row, &col) == 2) {
                char *v = rest;
                while (*v == ' ') v++;
                while (*v >= '0' && *v <= '9') v++;
                while (*v == ' ') v++;
                while (*v >= '0' && *v <= '9') v++;
                while (*v == ' ') v++;
                table_set(name, row, col, v);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            int row, col;
            if (sscanf(rest, "%d %d", &row, &col) == 2) {
                table_get(name, row, col);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            table_delete(name);
        } else if (strcmp(cmd, "TABLES") == 0) {
            table_tables();
        }
    }
    return 0;
}