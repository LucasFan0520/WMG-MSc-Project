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
    if (rows <= 0 || cols <= 0) return;
    Table *t = malloc(sizeof(Table));
    t->name = malloc(strlen(name) + 1);
    strcpy(t->name, name);
    t->rows = rows;
    t->cols = cols;
    t->cells = malloc(rows * sizeof(char **));
    for (int i = 0; i < rows; i++) {
        t->cells[i] = malloc(cols * sizeof(char *));
        for (int j = 0; j < cols; j++) {
            t->cells[i][j] = malloc(1);
            t->cells[i][j][0] = '\0';
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
    t->cells[row][col] = malloc(strlen(value) + 1);
    strcpy(t->cells[row][col], value);
}

void get_cell(const char *name, int row, int col) {
    Table *t = find_table(name);
    if (!t) { printf("EMPTY\n"); return; }
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) {
        printf("EMPTY\n");
        return;
    }
    if (strlen(t->cells[row][col]) == 0) printf("EMPTY\n");
    else printf("%s\n", t->cells[row][col]);
}

void delete_table(const char *name) {
    Table *t = tables, *prev = NULL;
    while (t) {
        if (strcmp(t->name, name) == 0) {
            if (prev) prev->next = t->next;
            else tables = t->next;
            free(t->name);
            for (int i = 0; i < t->rows; i++) {
                for (int j = 0; j < t->cols; j++) free(t->cells[i][j]);
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
    Table *t = tables;
    while (t) {
        printf("%s\n", t->name);
        t = t->next;
    }
}

void free_all(void) {
    Table *t = tables;
    while (t) {
        Table *tmp = t->next;
        free(t->name);
        for (int i = 0; i < t->rows; i++) {
            for (int j = 0; j < t->cols; j++) free(t->cells[i][j]);
            free(t->cells[i]);
        }
        free(t->cells);
        free(t);
        t = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], value[4096];
        int rows, cols, row, col;
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "TABLE") == 0) {
            if (sscanf(rest, "%255s %d %d", name, &rows, &cols) == 3) {
                create_table(name, rows, cols);
            }
        } else if (strcmp(cmd, "SET") == 0) {
            if (sscanf(rest, "%255s %d %d", name, &row, &col) == 3) {
                const char *v = rest;
                while (*v && *v != ' ') v++;
                while (*v == ' ') v++;
                while (*v && *v != ' ') v++;
                while (*v == ' ') v++;
                while (*v && *v != ' ') v++;
                while (*v == ' ') v++;
                if (*v) set_cell(name, row, col, v);
                else set_cell(name, row, col, "");
            }
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(rest, "%255s %d %d", name, &row, &col) == 3) {
                get_cell(name, row, col);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(rest, "%255s", name) == 1) delete_table(name);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    free_all();
    return 0;
}