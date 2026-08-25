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
    if (!t) return;
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = malloc(rows * sizeof(char**));
    if (!t->cells) { free(t->name); free(t); return; }
    for (int i = 0; i < rows; i++) {
        t->cells[i] = malloc(cols * sizeof(char*));
        if (!t->cells[i]) {
            for (int j = 0; j < i; j++) free(t->cells[j]);
            free(t->cells);
            free(t->name);
            free(t);
            return;
        }
        for (int j = 0; j < cols; j++) t->cells[i][j] = NULL;
    }
    t->next = tables;
    tables = t;
}

void set_cell(const char *name, int row, int col, const char *value) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    if (t->cells[row][col]) free(t->cells[row][col]);
    t->cells[row][col] = strdup(value);
}

void get_cell(const char *name, int row, int col) {
    Table *t = find_table(name);
    if (!t) { printf("EMPTY\n"); return; }
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) {
        printf("EMPTY\n");
        return;
    }
    char *val = t->cells[row][col];
    printf("%s\n", val ? val : "EMPTY");
}

void table_delete(const char *name) {
    Table *t = tables, *prev = NULL;
    while (t) {
        if (strcmp(t->name, name) == 0) {
            if (prev) prev->next = t->next;
            else tables = t->next;
            free(t->name);
            for (int i = 0; i < t->rows; i++) {
                for (int j = 0; j < t->cols; j++) {
                    free(t->cells[i][j]);
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

void tables_list(void) {
    Table *t = tables;
    while (t) {
        printf("%s\n", t->name);
        t = t->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[256], arg4[2048];
        int n = sscanf(line, "%15s %255s %255s %255s %2047s", cmd, arg1, arg2, arg3, arg4);
        if (strcmp(cmd, "TABLE") == 0 && n >= 4) {
            table_create(arg1, atoi(arg2), atoi(arg3));
        } else if (strcmp(cmd, "SET") == 0 && n >= 5) {
            set_cell(arg1, atoi(arg2), atoi(arg3), arg4);
        } else if (strcmp(cmd, "GET") == 0 && n >= 4) {
            get_cell(arg1, atoi(arg2), atoi(arg3));
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            table_delete(arg1);
        } else if (strcmp(cmd, "TABLES") == 0) {
            tables_list();
        }
    }
    while (tables) {
        Table *tmp = tables;
        tables = tables->next;
        free(tmp->name);
        for (int i = 0; i < tmp->rows; i++) {
            for (int j = 0; j < tmp->cols; j++) free(tmp->cells[i][j]);
            free(tmp->cells[i]);
        }
        free(tmp->cells);
        free(tmp);
    }
    return 0;
}