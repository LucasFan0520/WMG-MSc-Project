// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Table {
    char *name;
    int rows;
    int cols;
    char ***cells; // rows x cols
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
    t->cells = malloc(rows * sizeof(char **));
    for (int i = 0; i < rows; i++) {
        t->cells[i] = malloc(cols * sizeof(char *));
        for (int j = 0; j < cols; j++) {
            t->cells[i][j] = strdup(""); // empty string
        }
    }
    t->next = tables;
    tables = t;
}

void set_cell(const char *name, int row, int col, const char *value) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    free(t->cells[row][col]);
    t->cells[row][col] = strdup(value);
}

void get_cell(const char *name, int row, int col) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    if (strlen(t->cells[row][col]) == 0) printf("EMPTY\n");
    else printf("%s\n", t->cells[row][col]);
}

void delete_table(const char *name) {
    Table *cur = tables, *prev = NULL;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else tables = cur->next;
            free(cur->name);
            for (int i = 0; i < cur->rows; i++) {
                for (int j = 0; j < cur->cols; j++) free(cur->cells[i][j]);
                free(cur->cells[i]);
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

void cleanup_tables(void) {
    Table *t = tables;
    while (t) {
        Table *tmp = t;
        t = t->next;
        free(tmp->name);
        for (int i = 0; i < tmp->rows; i++) {
            for (int j = 0; j < tmp->cols; j++) free(tmp->cells[i][j]);
            free(tmp->cells[i]);
        }
        free(tmp->cells);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256], rest[4096];
        int n = sscanf(line, "%31s %255s %4095s", cmd, name, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "TABLE") == 0 && n >= 3) {
            int rows, cols;
            if (sscanf(rest, "%d %d", &rows, &cols) == 2) {
                create_table(name, rows, cols);
            }
        } else if (strcmp(cmd, "SET") == 0 && n >= 3) {
            int row, col;
            char *value = rest;
            if (sscanf(rest, "%d %d", &row, &col) == 2) {
                char *val = rest;
                while (*val && *val != ' ') val++;
                while (*val && *val == ' ') val++;
                while (*val && *val != ' ') val++;
                while (*val && *val == ' ') val++;
                set_cell(name, row, col, val);
            }
        } else if (strcmp(cmd, "GET") == 0 && n >= 3) {
            int row, col;
            if (sscanf(rest, "%d %d", &row, &col) == 2) {
                get_cell(name, row, col);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_table(name);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    cleanup_tables();
    return 0;
}