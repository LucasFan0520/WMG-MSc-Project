/* F039.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct cell {
    char *value;
} cell_t;

typedef struct table {
    char *name;
    int rows;
    int cols;
    cell_t *cells; // flattened rows*cols
    struct table *next;
} table_t;

table_t *tables = NULL;
table_t *tail = NULL;

table_t *find_table(const char *name) {
    table_t *t = tables;
    while (t) {
        if (strcmp(t->name, name) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void table_cmd(const char *name, int rows, int cols) {
    if (find_table(name)) return;
    if (rows <= 0 || cols <= 0) return;
    table_t *t = malloc(sizeof(table_t));
    if (!t) return;
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = calloc(rows * cols, sizeof(cell_t));
    if (!t->cells) { free(t->name); free(t); return; }
    t->next = NULL;
    if (tail) tail->next = t;
    else tables = t;
    tail = t;
}

void set_cell(const char *name, int row, int col, const char *value) {
    table_t *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    int idx = row * t->cols + col;
    if (t->cells[idx].value) {
        free(t->cells[idx].value);
    }
    t->cells[idx].value = strdup(value);
}

void get_cell(const char *name, int row, int col) {
    table_t *t = find_table(name);
    if (!t) {
        printf("EMPTY\n");
        return;
    }
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) {
        printf("EMPTY\n");
        return;
    }
    int idx = row * t->cols + col;
    if (t->cells[idx].value) {
        printf("%s\n", t->cells[idx].value);
    } else {
        printf("EMPTY\n");
    }
}

void delete_table(const char *name) {
    table_t *prev = NULL, *t = tables;
    while (t) {
        if (strcmp(t->name, name) == 0) {
            if (prev) prev->next = t->next;
            else tables = t->next;
            if (t == tail) tail = prev;
            free(t->name);
            for (int i = 0; i < t->rows * t->cols; i++) {
                free(t->cells[i].value);
            }
            free(t->cells);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void tables_list(void) {
    table_t *t = tables;
    while (t) {
        printf("%s\n", t->name);
        t = t->next;
    }
}

void free_all(void) {
    table_t *t = tables;
    while (t) {
        table_t *nxt = t->next;
        free(t->name);
        for (int i = 0; i < t->rows * t->cols; i++) {
            free(t->cells[i].value);
        }
        free(t->cells);
        free(t);
        t = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256];
        int rows, cols;
        if (sscanf(line, "%9s %255s %d %d", cmd, a, &rows, &cols) >= 2) {
            const char *rest = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "TABLE") == 0) {
                if (sscanf(line, "%*s %255s %d %d", a, &rows, &cols) == 3)
                    table_cmd(a, rows, cols);
            } else if (strcmp(cmd, "SET") == 0) {
                int row, col;
                if (sscanf(line, "%*s %255s %d %d", a, &row, &col) == 3) {
                    const char *val = line + strlen(cmd) + 1 + strlen(a) + 1;
                    while (*val == ' ') val++;
                    set_cell(a, row, col, val);
                }
            } else if (strcmp(cmd, "GET") == 0) {
                int row, col;
                if (sscanf(line, "%*s %255s %d %d", a, &row, &col) == 3)
                    get_cell(a, row, col);
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_table(a);
            } else if (strcmp(cmd, "TABLES") == 0) {
                tables_list();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}