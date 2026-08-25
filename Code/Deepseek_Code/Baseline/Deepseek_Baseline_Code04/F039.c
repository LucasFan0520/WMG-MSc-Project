/* F039.c */
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
    Cell *cells;
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
    if (rows <= 0 || cols <= 0) return;
    if (find_table(name)) return;
    Table *t = malloc(sizeof(Table));
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = calloc(rows * cols, sizeof(Cell));
    for (int i = 0; i < rows * cols; i++) t->cells[i].value = strdup("");
    t->next = tables;
    tables = t;
}

void set_cell(const char *name, int row, int col, const char *value) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    int idx = row * t->cols + col;
    free(t->cells[idx].value);
    t->cells[idx].value = strdup(value);
}

void get_cell(const char *name, int row, int col) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    int idx = row * t->cols + col;
    if (strlen(t->cells[idx].value) == 0) printf("EMPTY\n");
    else printf("%s\n", t->cells[idx].value);
}

void delete_table(const char *name) {
    Table *prev = NULL, *cur = tables;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else tables = cur->next;
            free(cur->name);
            for (int i = 0; i < cur->rows * cur->cols; i++) {
                free(cur->cells[i].value);
            }
            free(cur->cells);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void list_tables(void) {
    Table *t = tables;
    while (t) {
        printf("%s\n", t->name);
        t = t->next;
    }
}

void free_tables(void) {
    while (tables) {
        Table *tmp = tables;
        tables = tables->next;
        free(tmp->name);
        for (int i = 0; i < tmp->rows * tmp->cols; i++) {
            free(tmp->cells[i].value);
        }
        free(tmp->cells);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char name[256];
        int rows, cols;
        int n = sscanf(line, "%s %s %d %d", cmd, name, &rows, &cols);
        if (strcmp(cmd, "TABLE") == 0) {
            if (n < 4) continue;
            create_table(name, rows, cols);
        } else if (strcmp(cmd, "SET") == 0) {
            if (n < 4) continue;
            char *p = line + strlen(cmd) + 1 + strlen(name) + 1;
            while (*p == ' ') p++;
            int r = atoi(p);
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            int c = atoi(p);
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            if (*p) set_cell(name, r, c, p);
        } else if (strcmp(cmd, "GET") == 0) {
            if (n < 4) continue;
            get_cell(name, rows, cols);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n < 2) continue;
            delete_table(name);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    free_tables();
    return 0;
}