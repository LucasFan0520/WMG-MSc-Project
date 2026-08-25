// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Table {
    char *name;
    int rows;
    int cols;
    char ***cells; // cells[rows][cols]
    struct Table *next;
} Table;

Table *head = NULL;

Table* find_table(const char *name) {
    Table *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void create_table(const char *name, int rows, int cols) {
    if (find_table(name)) return;
    if (rows < 0 || cols < 0) return;
    Table *t = malloc(sizeof(Table));
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = malloc(rows * sizeof(char**));
    for (int i = 0; i < rows; i++) {
        t->cells[i] = malloc(cols * sizeof(char*));
        for (int j = 0; j < cols; j++) {
            t->cells[i][j] = strdup("");
        }
    }
    t->next = NULL;
    if (!head) head = t;
    else {
        Table *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = t;
    }
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
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) {
        printf("EMPTY\n");
        return;
    }
    printf("%s\n", t->cells[row][col]);
}

void delete_table(const char *name) {
    Table *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->name);
            for (int i = 0; i < cur->rows; i++) {
                for (int j = 0; j < cur->cols; j++) {
                    free(cur->cells[i][j]);
                }
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

void list_tables() {
    Table *cur = head;
    while (cur) {
        printf("%s\n", cur->name);
        cur = cur->next;
    }
}

void free_all() {
    Table *cur = head;
    while (cur) {
        Table *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        for (int i = 0; i < tmp->rows; i++) {
            for (int j = 0; j < tmp->cols; j++) {
                free(tmp->cells[i][j]);
            }
            free(tmp->cells[i]);
        }
        free(tmp->cells);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "TABLE") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *rows_str = strtok(NULL, " ");
            if (!rows_str) continue;
            char *cols_str = strtok(NULL, " ");
            if (!cols_str) continue;
            int rows = atoi(rows_str);
            int cols = atoi(cols_str);
            create_table(name, rows, cols);
        } else if (strcmp(cmd, "SET") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *row_str = strtok(NULL, " ");
            if (!row_str) continue;
            char *col_str = strtok(NULL, " ");
            if (!col_str) continue;
            char *val = strtok(NULL, "");
            if (!val) val = "";
            int row = atoi(row_str);
            int col = atoi(col_str);
            set_cell(name, row, col, val);
        } else if (strcmp(cmd, "GET") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *row_str = strtok(NULL, " ");
            if (!row_str) continue;
            char *col_str = strtok(NULL, " ");
            if (!col_str) continue;
            int row = atoi(row_str);
            int col = atoi(col_str);
            get_cell(name, row, col);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_table(name);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    free_all();
    return 0;
}