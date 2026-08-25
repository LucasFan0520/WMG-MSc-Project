// F039.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    size_t rows;
    size_t cols;
    char ***cells;
} CountedTable;

int main(void) {
    CountedTable *tables = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "TABLE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *r_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *c_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*name == '\0' || *r_str == '\0' || *c_str == '\0') continue;
            long r = strtol(r_str, NULL, 10);
            long c = strtol(c_str, NULL, 10);
            if (r <= 0 || c <= 0 || r > 500 || c > 500) continue;

            char ***grid = calloc(r, sizeof(char **));
            if (!grid) continue;
            for (long i = 0; i < r; i++) {
                grid[i] = calloc(c, sizeof(char *));
            }

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                CountedTable *new_tables = realloc(tables, new_cap * sizeof(CountedTable));
                if (new_tables) {
                    tables = new_tables;
                    capacity = new_cap;
                } else {
                    for (long i = 0; i < r; i++) free(grid[i]);
                    free(grid);
                    continue;
                }
            }
            char *nm = strdup(name);
            if (nm) {
                tables[count].name = nm;
                tables[count].rows = (size_t)r;
                tables[count].cols = (size_t)c;
                tables[count].cells = grid;
                count++;
            } else {
                for (long i = 0; i < r; i++) free(grid[i]);
                free(grid);
            }
        } else if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *r_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *c_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *val = p;

            if (*name == '\0' || *r_str == '\0' || *c_str == '\0') continue;
            long row = strtol(r_str, NULL, 10);
            long col = strtol(c_str, NULL, 10);

            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    if (row >= 0 && (size_t)row < tables[i].rows && col >= 0 && (size_t)col < tables[i].cols) {
                        char *v = strdup(val);
                        if (v) {
                            free(tables[i].cells[row][col]);
                            tables[i].cells[row][col] = v;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *r_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *c_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*name == '\0' || *r_str == '\0' || *c_str == '\0') continue;
            long row = strtol(r_str, NULL, 10);
            long col = strtol(c_str, NULL, 10);

            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    if (row >= 0 && (size_t)row < tables[i].rows && col >= 0 && (size_t)col < tables[i].cols) {
                        if (tables[i].cells[row][col]) {
                            printf("%s\n", tables[i].cells[row][col]);
                        } else {
                            printf("EMPTY\n");
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    free(tables[i].name);
                    for (size_t r = 0; r < tables[i].rows; r++) {
                        for (size_t c = 0; c < tables[i].cols; c++) {
                            free(tables[i].cells[r][c]);
                        }
                        free(tables[i].cells[r]);
                    }
                    free(tables[i].cells);
                    for (size_t j = i; j < count - 1; j++) {
                        tables[j] = tables[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", tables[i].name);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(tables[i].name);
        for (size_t r = 0; r < tables[i].rows; r++) {
            for (size_t c = 0; c < tables[i].cols; c++) {
                free(tables[i].cells[r][c]);
            }
            free(tables[i].cells[r]);
        }
        free(tables[i].cells);
    }
    free(tables);
    free(line);
    return 0;
}
