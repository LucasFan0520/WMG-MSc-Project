/* F039.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct table {
    char *name;
    int rows;
    int cols;
    char ***cells; // cells[rows][cols]
    struct table *next;
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
    if (rows < 0 || cols < 0) return;
    Table *t = malloc(sizeof(Table));
    t->name = strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = malloc(rows * sizeof(char **));
    for (int i = 0; i < rows; i++) {
        t->cells[i] = calloc(cols, sizeof(char *));
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
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) { printf("EMPTY\n"); return; }
    if (t->cells[row][col]) {
        for (char *c = t->cells[row][col]; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
    } else {
        printf("EMPTY\n");
    }
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

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], name[256], value[1024];
        int rows, cols, row, col;
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "TABLES") == 0) { list_tables(); continue; }
        if (strcmp(cmd, "TABLE") == 0) {
            if (sscanf(line, "%*s %255s %d %d", name, &rows, &cols) == 3)
                create_table(name, rows, cols);
        } else if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %d %d", name, &row, &col) < 3) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(name) + 1 + (row >= 0 ? snprintf(NULL,0,"%d",row) : 0) + 1 + (col >= 0 ? snprintf(NULL,0,"%d",col) : 0) + 1;
            if (*rest == ' ') rest++;
            set_cell(name, row, col, rest);
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s %d %d", name, &row, &col) == 3)
                get_cell(name, row, col);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) delete_table(name);
        }
    }
    return 0;
}