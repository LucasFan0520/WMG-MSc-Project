// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Cell {
    char *value;
} Cell;

typedef struct Table {
    char *name;
    int rows;
    int cols;
    Cell **cells; // rows * cols
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

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void table_create(const char *name, int rows, int cols) {
    if (find_table(name)) return;
    if (rows <= 0 || cols <= 0) return;
    Table *t = malloc(sizeof(Table));
    if (!t) return;
    t->name = safe_strdup(name);
    t->rows = rows;
    t->cols = cols;
    t->cells = calloc(rows * cols, sizeof(Cell));
    if (!t->cells) { free(t->name); free(t); return; }
    for (int i = 0; i < rows * cols; i++) {
        t->cells[i].value = safe_strdup("");
    }
    t->next = tables;
    tables = t;
}

void table_set(const char *name, int row, int col, const char *value) {
    Table *t = find_table(name);
    if (!t) return;
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) return;
    int index = row * t->cols + col;
    free(t->cells[index].value);
    t->cells[index].value = safe_strdup(value);
}

void table_get(const char *name, int row, int col) {
    Table *t = find_table(name);
    if (!t) { printf("EMPTY\n"); return; }
    if (row < 0 || row >= t->rows || col < 0 || col >= t->cols) { printf("EMPTY\n"); return; }
    int index = row * t->cols + col;
    if (t->cells[index].value && strlen(t->cells[index].value) > 0)
        printf("%s\n", t->cells[index].value);
    else
        printf("EMPTY\n");
}

void table_delete(const char *name) {
    Table *prev = NULL, *cur = tables;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else tables = cur->next;
            free(cur->name);
            for (int i = 0; i < cur->rows * cur->cols; i++) {
                free(cur->cells[i].value);
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

void free_all(void) {
    Table *t = tables;
    while (t) {
        Table *next = t->next;
        free(t->name);
        for (int i = 0; i < t->rows * t->cols; i++) {
            free(t->cells[i].value);
        }
        free(t->cells);
        free(t);
        t = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512], arg3[512];
        int n = sscanf(line, "%31s %511s %511s %511s", cmd, arg1, arg2, arg3);
        if (n < 1) continue;

        if (strcmp(cmd, "TABLE") == 0 && n >= 4) {
            int rows = atoi(arg2);
            int cols = atoi(arg3);
            table_create(arg1, rows, cols);
        } else if (strcmp(cmd, "SET") == 0 && n >= 5) {
            int row = atoi(arg2);
            int col = atoi(arg3);
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            table_set(arg1, row, col, p);
        } else if (strcmp(cmd, "GET") == 0 && n >= 4) {
            int row = atoi(arg2);
            int col = atoi(arg3);
            table_get(arg1, row, col);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            table_delete(arg1);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_tables();
        }
    }
    free_all();
    return 0;
}