// F039.c
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
    Cell **cells; // rows x cols, flattened
    struct Table *next;
} Table;

Table *head = NULL;

Table *find_table(const char *name) {
    Table *t = head;
    while (t) {
        if (strcmp(t->name, name) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void create_table(const char *name, int rows, int cols) {
    if (find_table(name)) return;
    if (rows <= 0 || cols <= 0) return;
    Table *t = malloc(sizeof(Table));
    if (!t) return;
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = calloc(rows * cols, sizeof(Cell *));
    t->next = NULL;
    if (!t->name || !t->cells) {
        free(t->name); free(t->cells); free(t);
        return;
    }
    if (!head) head = t;
    else {
        Table *last = head;
        while (last->next) last = last->next;
        last->next = t;
    }
}

void set_cell(const char *name, int row, int col, const char *value) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    int idx = row * t->cols + col;
    if (t->cells[idx]) {
        free(t->cells[idx]->value);
        free(t->cells[idx]);
        t->cells[idx] = NULL;
    }
    Cell *c = malloc(sizeof(Cell));
    if (!c) return;
    c->value = strdup(value);
    c->next = NULL;
    if (!c->value) { free(c); return; }
    t->cells[idx] = c;
}

void get_cell(const char *name, int row, int col) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    int idx = row * t->cols + col;
    if (t->cells[idx] && t->cells[idx]->value) {
        printf("%s\n", t->cells[idx]->value);
    } else {
        printf("EMPTY\n");
    }
}

void delete_table(const char *name) {
    Table *t = head, *prev = NULL;
    while (t) {
        if (strcmp(t->name, name) == 0) {
            if (prev) prev->next = t->next;
            else head = t->next;
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

void list_tables(void) {
    Table *t = head;
    while (t) {
        printf("%s\n", t->name);
        t = t->next;
    }
}

void free_all(void) {
    Table *t = head;
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
        char cmd[64], name[256], value[4096];
        int rows, cols, row, col;
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "TABLE") == 0) {
            if (sscanf(line, "%*s %255s %d %d", name, &rows, &cols) == 3)
                create_table(name, rows, cols);
        } else if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %d %d %4095[^\n]", name, &row, &col, value) == 4)
                set_cell(name, row, col, value);
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s %d %d", name, &row, &col) == 3)
                get_cell(name, row, col);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_table(name);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    free(line);
    free_all();
    return 0;
}