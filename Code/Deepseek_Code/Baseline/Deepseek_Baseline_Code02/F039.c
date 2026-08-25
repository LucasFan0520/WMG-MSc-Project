// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Table {
    char *name;
    int rows;
    int cols;
    char ***cells; // cells[row][col]
    struct Table *next;
} Table;

Table *head = NULL;

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
    if (rows <= 0 || cols <= 0) return;
    Table *t = malloc(sizeof(Table));
    t->name = malloc(strlen(name) + 1);
    strcpy(t->name, name);
    t->rows = rows;
    t->cols = cols;
    t->cells = malloc(rows * sizeof(char**));
    for (int i = 0; i < rows; i++) {
        t->cells[i] = malloc(cols * sizeof(char*));
        for (int j = 0; j < cols; j++) {
            t->cells[i][j] = malloc(1);
            t->cells[i][j][0] = 0; // empty string
        }
    }
    t->next = head;
    head = t;
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
    int first = 1;
    while (t) {
        if (!first) putchar(' ');
        printf("%s", t->name);
        first = 0;
        t = t->next;
    }
    putchar('\n');
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "TABLE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            int rows, cols;
            if (sscanf(p, "%d %d", &rows, &cols) == 2) {
                create_table(name, rows, cols);
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            int row, col;
            if (sscanf(p, "%d %d", &row, &col) != 2) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            set_cell(name, row, col, p);
        } else if (strcmp(cmd, "GET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            int row, col;
            if (sscanf(p, "%d %d", &row, &col) == 2) {
                get_cell(name, row, col);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_table(p);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    free_all();
    return 0;
}