/* F039.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Table {
    char *name;
    int rows;
    int cols;
    char ***cells;
    struct Table *next;
} Table;

Table *tables = NULL;

Table* find_table(const char *name) {
    Table *t = tables;
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
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = malloc(rows * sizeof(char**));
    for (int i = 0; i < rows; i++) {
        t->cells[i] = calloc(cols, sizeof(char*));
    }
    t->next = tables;
    tables = t;
}

void set_cell(const char *name, int row, int col, const char *value) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    if (t->cells[row][col]) {
        free(t->cells[row][col]);
    }
    t->cells[row][col] = strdup(value);
}

void get_cell(const char *name, int row, int col) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) {
        printf("EMPTY\n");
        return;
    }
    char *val = t->cells[row][col];
    if (val) printf("%s\n", val);
    else printf("EMPTY\n");
}

void delete_table(const char *name) {
    Table *t = tables, *prev = NULL;
    while (t) {
        if (strcmp(t->name, name) == 0) {
            if (prev) prev->next = t->next;
            else tables = t->next;
            free(t->name);
            for (int i = 0; i < t->rows; i++) {
                for (int j = 0; j < t->cols; j++) {
                    if (t->cells[i][j]) free(t->cells[i][j]);
                }
                free(t->cells[i]);
            }
            free(t->cells);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void list_tables() {
    Table *t = tables;
    while (t) {
        printf("%s\n", t->name);
        t = t->next;
    }
}

void free_all() {
    while (tables) {
        Table *tmp = tables;
        tables = tables->next;
        free(tmp->name);
        for (int i = 0; i < tmp->rows; i++) {
            for (int j = 0; j < tmp->cols; j++) {
                if (tmp->cells[i][j]) free(tmp->cells[i][j]);
            }
            free(tmp->cells[i]);
        }
        free(tmp->cells);
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], value[4096];
        int rows, cols, row, col;
        int n = sscanf(line, "%31s %255s %d %d %[^\n]", cmd, name, &rows, &cols, value);
        if (strcmp(cmd, "TABLE") == 0 && n >= 4) {
            create_table(name, rows, cols);
        } else if (strcmp(cmd, "SET") == 0 && n >= 5) {
            set_cell(name, rows, cols, value);
        } else if (strcmp(cmd, "GET") == 0 && n >= 4) {
            get_cell(name, rows, cols);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_table(name);
        } else if (strcmp(cmd, "TABLES") == 0 && n == 1) {
            list_tables();
        }
    }
    free_all();
    return 0;
}