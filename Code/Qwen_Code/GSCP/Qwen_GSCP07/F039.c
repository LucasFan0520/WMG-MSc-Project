// F039.c
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

Table *find_table(const char *name) {
    for (Table *t = tables; t; t = t->next) {
        if (strcmp(t->name, name) == 0) return t;
    }
    return NULL;
}

void create_table(const char *name, int rows, int cols) {
    if (find_table(name) || rows < 0 || cols < 0) return;
    Table *t = malloc(sizeof(Table));
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = malloc(rows * sizeof(char **));
    for (int i = 0; i < rows; i++) {
        t->cells[i] = malloc(cols * sizeof(char *));
        for (int j = 0; j < cols; j++) {
            t->cells[i][j] = NULL;
        }
    }
    t->next = tables;
    tables = t;
}

void set_cell(const char *name, int row, int col, const char *value) {
    Table *t = find_table(name);
    if (!t || row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    free(t->cells[row][col]);
    t->cells[row][col] = strdup(value);
}

void get_cell(const char *name, int row, int col) {
    Table *t = find_table(name);
    if (!t || row < 0 || row >= t->rows || col < 0 || col >= t->cols) {
        printf("EMPTY\n");
        return;
    }
    if (t->cells[row][col]) printf("%s\n", t->cells[row][col]);
    else printf("EMPTY\n");
}

void delete_table(const char *name) {
    Table **tp = &tables;
    while (*tp) {
        if (strcmp((*tp)->name, name) == 0) {
            Table *tmp = *tp;
            *tp = tmp->next;
            for (int i = 0; i < tmp->rows; i++) {
                for (int j = 0; j < tmp->cols; j++) {
                    free(tmp->cells[i][j]);
                }
                free(tmp->cells[i]);
            }
            free(tmp->cells);
            free(tmp->name);
            free(tmp);
            return;
        }
        tp = &(*tp)->next;
    }
}

void list_tables() {
    for (Table *t = tables; t; t = t->next) {
        printf("%s\n", t->name);
    }
}

void cleanup() {
    while (tables) {
        Table *tmp = tables->next;
        for (int i = 0; i < tables->rows; i++) {
            for (int j = 0; j < tables->cols; j++) {
                free(tables->cells[i][j]);
            }
            free(tables->cells[i]);
        }
        free(tables->cells);
        free(tables->name);
        free(tables);
        tables = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256];
        int r, c;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "TABLE") == 0) {
            if (sscanf(line, "%*s %255s %d %d", a1, &r, &c) == 3) create_table(a1, r, c);
        } else if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %d %d", a1, &r, &c) == 3) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) { t = strchr(t + 1, ' '); if (t) { t = strchr(t + 1, ' '); if (t) set_cell(a1, r, c, t + 1); } } }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s %d %d", a1, &r, &c) == 3) get_cell(a1, r, c);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) delete_table(a1);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    cleanup();
    return 0;
}