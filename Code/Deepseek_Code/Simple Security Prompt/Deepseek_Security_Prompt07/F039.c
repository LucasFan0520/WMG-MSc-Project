// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Cell {
    char *value;
} Cell;

typedef struct Table {
    char *name;
    int rows;
    int cols;
    Cell *cells; // flattened rows*cols
    struct Table *next;
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

Cell *get_cell(Table *t, int row, int col) {
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return NULL;
    return &t->cells[row * t->cols + col];
}

void create_table(const char *name, int rows, int cols) {
    if (find_table(name) || rows < 0 || cols < 0) return;
    Table *t = malloc(sizeof(Table));
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = calloc(rows * cols, sizeof(Cell));
    for (int i = 0; i < rows * cols; i++) {
        t->cells[i].value = strdup("");
    }
    t->next = tables;
    tables = t;
}

void set_cell(const char *name, int row, int col, const char *value) {
    Table *t = find_table(name);
    if (!t) return;
    Cell *c = get_cell(t, row, col);
    if (!c) return;
    free(c->value);
    c->value = strdup(value);
}

void get_cell_value(const char *name, int row, int col) {
    Table *t = find_table(name);
    if (!t) {
        printf("EMPTY\n");
        return;
    }
    Cell *c = get_cell(t, row, col);
    if (!c || c->value[0] == '\0') {
        printf("EMPTY\n");
        return;
    }
    for (char *s = c->value; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void delete_table(const char *name) {
    Table *prev = NULL;
    Table *t = tables;
    while (t) {
        if (strcmp(t->name, name) == 0) {
            if (prev) prev->next = t->next;
            else tables = t->next;
            for (int i = 0; i < t->rows * t->cols; i++) {
                free(t->cells[i].value);
            }
            free(t->cells);
            free(t->name);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void list_table_names() {
    Table *t = tables;
    int first = 1;
    while (t) {
        if (!first) putchar(' ');
        printf("%s", t->name);
        first = 0;
        t = t->next;
    }
    putchar('\n');
}

void free_all() {
    Table *t = tables;
    while (t) {
        Table *next = t->next;
        for (int i = 0; i < t->rows * t->cols; i++) {
            free(t->cells[i].value);
        }
        free(t->cells);
        free(t->name);
        free(t);
        t = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "TABLE") == 0) {
            char name[256];
            int rows, cols;
            if (sscanf(rest, "%255s %d %d", name, &rows, &cols) == 3) {
                create_table(name, rows, cols);
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char name[256];
            int row, col;
            char value[4096];
            if (sscanf(rest, "%255s %d %d %[^\n]", name, &row, &col, value) == 4) {
                set_cell(name, row, col, value);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char name[256];
            int row, col;
            if (sscanf(rest, "%255s %d %d", name, &row, &col) == 3) {
                get_cell_value(name, row, col);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_table(rest);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_table_names();
        }
    }
    free_all();
    return 0;
}