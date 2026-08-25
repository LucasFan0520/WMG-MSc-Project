// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct cell {
    char *value;
    int row, col;
    struct cell *next;
} Cell;

typedef struct table {
    char *name;
    int rows, cols;
    Cell *cells;
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

Cell *find_cell(Table *t, int row, int col) {
    Cell *c = t->cells;
    while (c) {
        if (c->row == row && c->col == col) return c;
        c = c->next;
    }
    return NULL;
}

void set_cell(Table *t, int row, int col, const char *value) {
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    Cell *c = find_cell(t, row, col);
    if (c) {
        free(c->value);
        c->value = strdup(value);
        return;
    }
    Cell *new = malloc(sizeof(Cell));
    new->value = strdup(value);
    new->row = row;
    new->col = col;
    new->next = t->cells;
    t->cells = new;
}

void get_cell(Table *t, int row, int col) {
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    Cell *c = find_cell(t, row, col);
    if (c) printf("%s\n", c->value);
    else printf("EMPTY\n");
}

void table_create(const char *name, int rows, int cols) {
    if (find_table(name)) return;
    Table *t = malloc(sizeof(Table));
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = NULL;
    t->next = tables;
    tables = t;
}

void delete_table(const char *name) {
    Table *t = tables, *prev = NULL;
    while (t) {
        if (strcmp(t->name, name) == 0) {
            if (prev) prev->next = t->next;
            else tables = t->next;
            free(t->name);
            Cell *c = t->cells;
            while (c) { Cell *tmp = c; c = c->next; free(tmp->value); free(tmp); }
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void list_tables(void) {
    Table *t = tables;
    while (t) {
        printf("%s\n", t->name);
        t = t->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], value[4096];
        int rows, cols, r, c;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "TABLE") == 0) {
            if (sscanf(line, "%*s %255s %d %d", name, &rows, &cols) == 3)
                table_create(name, rows, cols);
        } else if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %d %d %[^\n]", name, &r, &c, value) == 4) {
                Table *t = find_table(name);
                if (t) set_cell(t, r, c, value);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s %d %d", name, &r, &c) == 3) {
                Table *t = find_table(name);
                if (t) get_cell(t, r, c);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_table(name);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    return 0;
}