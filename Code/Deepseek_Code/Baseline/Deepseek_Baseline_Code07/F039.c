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

Table *head = NULL;
Table *tail = NULL;

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
    Table *t = malloc(sizeof(Table));
    t->name = malloc(strlen(name)+1);
    strcpy(t->name, name);
    t->rows = rows;
    t->cols = cols;
    t->cells = malloc(rows * sizeof(char**));
    for (int i = 0; i < rows; i++) {
        t->cells[i] = malloc(cols * sizeof(char*));
        for (int j = 0; j < cols; j++) {
            t->cells[i][j] = malloc(1);
            t->cells[i][j][0] = 0;
        }
    }
    t->next = NULL;
    if (tail) tail->next = t;
    else head = t;
    tail = t;
}

void set_cell(const char *name, int row, int col, const char *value) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    free(t->cells[row][col]);
    t->cells[row][col] = malloc(strlen(value)+1);
    strcpy(t->cells[row][col], value);
}

void get_cell(const char *name, int row, int col) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    if (strlen(t->cells[row][col]) == 0) printf("EMPTY\n");
    else printf("%s\n", t->cells[row][col]);
}

void delete_table(const char *name) {
    Table *prev = NULL;
    Table *t = head;
    while (t) {
        if (strcmp(t->name, name) == 0) {
            if (prev) prev->next = t->next;
            else head = t->next;
            if (t == tail) tail = prev;
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
        for (int i = 0; i < t->rows; i++) {
            for (int j = 0; j < t->cols; j++) {
                free(t->cells[i][j]);
            }
            free(t->cells[i]);
        }
        free(t->cells);
        free(t);
        t = next;
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "TABLE") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *rows_str = strtok(NULL, " ");
            char *cols_str = strtok(NULL, " ");
            if (rows_str && cols_str) {
                int rows = atoi(rows_str);
                int cols = atoi(cols_str);
                if (rows > 0 && cols > 0) create_table(name, rows, cols);
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *row_str = strtok(NULL, " ");
            char *col_str = strtok(NULL, " ");
            if (!row_str || !col_str) continue;
            int row = atoi(row_str);
            int col = atoi(col_str);
            char *value = strtok(NULL, "");
            if (!value) value = "";
            set_cell(name, row, col, value);
        } else if (strcmp(cmd, "GET") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *row_str = strtok(NULL, " ");
            char *col_str = strtok(NULL, " ");
            if (row_str && col_str) {
                get_cell(name, atoi(row_str), atoi(col_str));
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(rest, " ");
            if (name) delete_table(name);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    free_all();
    return 0;
}