// F039.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Table {
    char *name;
    size_t rows;
    size_t cols;
    char ***cells; // rows x cols
} Table;

static Table *tables = NULL;
static size_t count = 0;
static size_t capacity = 0;

static ssize_t find_table(const char *name) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(tables[i].name, name) == 0)
            return (ssize_t)i;
    return -1;
}

static void create_table(const char *name, size_t rows, size_t cols) {
    if (find_table(name) != -1) return;
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Table *new = realloc(tables, capacity * sizeof(Table));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        tables = new;
    }
    tables[count].name = strdup(name);
    if (!tables[count].name) { fprintf(stderr, "Memory error\n"); exit(1); }
    tables[count].rows = rows;
    tables[count].cols = cols;
    tables[count].cells = malloc(rows * sizeof(char **));
    if (!tables[count].cells) { fprintf(stderr, "Memory error\n"); exit(1); }
    for (size_t r = 0; r < rows; r++) {
        tables[count].cells[r] = calloc(cols, sizeof(char *));
        if (!tables[count].cells[r]) { fprintf(stderr, "Memory error\n"); exit(1); }
    }
    count++;
}

static void set_cell(const char *name, size_t row, size_t col, const char *value) {
    ssize_t idx = find_table(name);
    if (idx == -1) return;
    if (row >= tables[idx].rows || col >= tables[idx].cols) return;
    if (tables[idx].cells[row][col])
        free(tables[idx].cells[row][col]);
    tables[idx].cells[row][col] = strdup(value);
    if (!tables[idx].cells[row][col]) { fprintf(stderr, "Memory error\n"); exit(1); }
}

static void get_cell(const char *name, size_t row, size_t col) {
    ssize_t idx = find_table(name);
    if (idx == -1) { printf("EMPTY\n"); return; }
    if (row >= tables[idx].rows || col >= tables[idx].cols) { printf("EMPTY\n"); return; }
    char *val = tables[idx].cells[row][col];
    if (val) printf("%s\n", val);
    else printf("EMPTY\n");
}

static void delete_table(const char *name) {
    ssize_t idx = find_table(name);
    if (idx == -1) return;
    free(tables[idx].name);
    for (size_t r = 0; r < tables[idx].rows; r++) {
        for (size_t c = 0; c < tables[idx].cols; c++)
            free(tables[idx].cells[r][c]);
        free(tables[idx].cells[r]);
    }
    free(tables[idx].cells);
    for (size_t i = idx; i < count - 1; i++)
        tables[i] = tables[i + 1];
    count--;
}

static void list_table_names(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s\n", tables[i].name);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(tables[i].name);
        for (size_t r = 0; r < tables[i].rows; r++) {
            for (size_t c = 0; c < tables[i].cols; c++)
                free(tables[i].cells[r][c]);
            free(tables[i].cells[r]);
        }
        free(tables[i].cells);
    }
    free(tables);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], name[256], value[4096];
    size_t rows, cols, row, col;

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "TABLE") == 0) {
            if (sscanf(line, "%*s %255s %zu %zu", name, &rows, &cols) == 3)
                create_table(name, rows, cols);
        } else if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %zu %zu %4095[^\n]", name, &row, &col, value) == 4)
                set_cell(name, row, col, value);
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s %zu %zu", name, &row, &col) == 3)
                get_cell(name, row, col);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_table(name);
        } else if (strcmp(cmd, "TABLES") == 0) {
            list_table_names();
        }
    }

    free(line);
    free_all();
    return 0;
}