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
    Cell **cells; // 2D array flattened
    struct Table *next;
} Table;

Table *head = NULL;

Table *find_table(char *name) {
    for (Table *t = head; t; t = t->next)
        if (strcmp(t->name, name) == 0) return t;
    return NULL;
}

void create_table(char *name, int rows, int cols) {
    if (find_table(name)) return;
    Table *t = malloc(sizeof(Table));
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = calloc(rows * cols, sizeof(Cell));
    for (int i = 0; i < rows * cols; i++) {
        t->cells[i].value = NULL;
    }
    t->next = head;
    head = t;
}

void set_cell(char *name, int row, int col, char *value) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    int idx = row * t->cols + col;
    if (t->cells[idx].value) free(t->cells[idx].value);
    t->cells[idx].value = strdup(value);
}

void get_cell(char *name, int row, int col) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    int idx = row * t->cols + col;
    if (t->cells[idx].value) printf("%s\n", t->cells[idx].value);
    else printf("EMPTY\n");
}

void delete_table(char *name) {
    Table *prev = NULL;
    for (Table *t = head; t; t = t->next) {
        if (strcmp(t->name, name) == 0) {
            if (prev) prev->next = t->next;
            else head = t->next;
            free(t->name);
            for (int i = 0; i < t->rows * t->cols; i++) {
                if (t->cells[i].value) free(t->cells[i].value);
            }
            free(t->cells);
            free(t);
            return;
        }
        prev = t;
    }
}

void list_tables(void) {
    for (Table *t = head; t; t = t->next)
        printf("%s\n", t->name);
}

void free_all(void) {
    Table *t = head;
    while (t) {
        Table *next = t->next;
        free(t->name);
        for (int i = 0; i < t->rows * t->cols; i++) {
            if (t->cells[i].value) free(t->cells[i].value);
        }
        free(t->cells);
        free(t);
        t = next;
    }
}

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[256], arg4[1024];
        int n = sscanf(line, "%15s %255s %255s %255s %[^\n]", cmd, arg1, arg2, arg3, arg4);
        if (n < 1) continue;
        if (strcmp(cmd, "TABLE") == 0 && n == 4) {
            create_table(arg1, atoi(arg2), atoi(arg3));
        } else if (strcmp(cmd, "SET") == 0 && n == 5) {
            set_cell(arg1, atoi(arg2), atoi(arg3), arg4);
        } else if (strcmp(cmd, "GET") == 0 && n == 4) {
            get_cell(arg1, atoi(arg2), atoi(arg3));
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_table(arg1);
        } else if (strcmp(cmd, "TABLES") == 0 && n == 1) {
            list_tables();
        }
    }
    free(line);
    free_all();
    return 0;
}