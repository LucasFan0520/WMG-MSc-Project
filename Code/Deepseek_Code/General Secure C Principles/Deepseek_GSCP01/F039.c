/* F039.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Cell {
    char *value;
    struct Cell *next;
} Cell;

typedef struct Table {
    char *name;
    int rows;
    int cols;
    Cell **cells;
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

void create_table(const char *name, int rows, int cols) {
    if (find_table(name)) return;
    Table *t = malloc(sizeof(Table));
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = calloc(rows * cols, sizeof(Cell *));
    t->next = tables;
    tables = t;
}

void set_cell(const char *name, int row, int col, const char *value) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    int idx = row * t->cols + col;
    if (t->cells[idx]) {
        free(t->cells[idx]->value);
        free(t->cells[idx]);
    }
    Cell *c = malloc(sizeof(Cell));
    c->value = strdup(value);
    c->next = NULL;
    t->cells[idx] = c;
}

void get_cell(const char *name, int row, int col) {
    Table *t = find_table(name);
    if (!t) { printf("EMPTY\n"); return; }
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) { printf("EMPTY\n"); return; }
    int idx = row * t->cols + col;
    if (!t->cells[idx]) { printf("EMPTY\n"); return; }
    printf("%s\n", t->cells[idx]->value);
}

void delete_table(const char *name) {
    Table *t = tables, *prev = NULL;
    while (t && strcmp(t->name, name) != 0) {
        prev = t;
        t = t->next;
    }
    if (!t) return;
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
}

void list_tables(void) {
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

void free_all(void) {
    Table *t = tables;
    while (t) {
        Table *next = t->next;
        free(t->name);
        for (int i = 0; i < t->rows * t->cols; i++) {
            if (t->cells[i]) {
                free(t->cells[i]->value);
                free(t->cells[i]);
            }
        }
        free(t->cells);
        free(t);
        t = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "TABLE") == 0) {
            char name[256];
            int rows, cols;
            if (sscanf(line + 6, "%255s %d %d", name, &rows, &cols) == 3)
                create_table(name, rows, cols);
        } else if (strcmp(cmd, "SET") == 0) {
            char name[256];
            int row, col;
            char *p = line + 4;
            if (sscanf(p, "%255s %d %d", name, &row, &col) != 3) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            set_cell(name, row, col, p);
        } else if (strcmp(cmd, "GET") == 0) {
            char name[256];
            int row, col;
            if (sscanf(line + 4, "%255s %d %d", name, &row, &col) == 3)
                get_cell(name, row, col);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_table(p);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    free_all();
    free(line);
    return 0;
}