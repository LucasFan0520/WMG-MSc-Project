// F039.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int rows;
    int cols;
    char ***cells;
} Table;

Table *tables = NULL;
int count = 0;
int capacity = 0;

int find_table(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(tables[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *name = space1 + 1;
            char *space2 = strchr(name, ' ');
            if (strcmp(cmd, "TABLE") == 0) {
                if (space2) {
                    *space2 = '\0';
                    char *rows_str = space2 + 1;
                    char *space3 = strchr(rows_str, ' ');
                    if (space3) {
                        *space3 = '\0';
                        int r = atoi(rows_str);
                        int c = atoi(space3 + 1);
                        if (r > 0 && c > 0 && find_table(name) == -1) {
                            char ***grid = malloc(r * sizeof(char**));
                            if (grid) {
                                int ok = 1;
                                for (int i = 0; i < r; i++) {
                                    grid[i] = calloc(c, sizeof(char*));
                                    if (!grid[i]) ok = 0;
                                }
                                if (ok) {
                                    if (count >= capacity) {
                                        capacity = capacity == 0 ? 4 : capacity * 2;
                                        Table *tmp = realloc(tables, capacity * sizeof(Table));
                                        if (!tmp) {
                                            free(line);
                                            return 1;
                                        }
                                        tables = tmp;
                                    }
                                    tables[count].name = strdup(name);
                                    tables[count].rows = r;
                                    tables[count].cols = c;
                                    tables[count].cells = grid;
                                    if (tables[count].name) {
                                        count++;
                                    }
                                }
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "SET") == 0) {
                if (space2) {
                    *space2 = '\0';
                    char *row_str = space2 + 1;
                    char *space3 = strchr(row_str, ' ');
                    if (space3) {
                        *space3 = '\0';
                        char *col_str = space3 + 1;
                        char *space4 = strchr(col_str, ' ');
                        if (space4) {
                            *space4 = '\0';
                            char *val = space4 + 1;
                            int r_idx = atoi(row_str);
                            int c_idx = atoi(col_str);
                            int tid = find_table(name);
                            if (tid != -1 && r_idx >= 0 && r_idx < tables[tid].rows && c_idx >= 0 && c_idx < tables[tid].cols) {
                                free(tables[tid].cells[r_idx][c_idx]);
                                tables[tid].cells[r_idx][c_idx] = strdup(val);
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "GET") == 0) {
                if (space2) {
                    *space2 = '\0';
                    char *row_str = space2 + 1;
                    char *space3 = strchr(row_str, ' ');
                    if (space3) {
                        *space3 = '\0';
                        int r_idx = atoi(row_str);
                        int c_idx = atoi(space3 + 1);
                        int tid = find_table(name);
                        if (tid != -1 && r_idx >= 0 && r_idx < tables[tid].rows && c_idx >= 0 && c_idx < tables[tid].cols) {
                            if (tables[tid].cells[r_idx][c_idx]) {
                                printf("%s\n", tables[tid].cells[r_idx][c_idx]);
                            } else {
                                printf("EMPTY\n");
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                int tid = find_table(name);
                if (tid != -1) {
                    free(tables[tid].name);
                    for (int i = 0; i < tables[tid].rows; i++) {
                        for (int j = 0; j < tables[tid].cols; j++) {
                            free(tables[tid].cells[i][j]);
                        }
                        free(tables[tid].cells[i]);
                    }
                    free(tables[tid].cells);
                    for (int i = tid; i < count - 1; i++) {
                        tables[i] = tables[i + 1];
                    }
                    count--;
                }
            }
        } else {
            if (strcmp(cmd, "TABLES") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", tables[i].name);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(tables[i].name);
        for (int j = 0; j < tables[i].rows; j++) {
            for (int k = 0; k < tables[i].cols; k++) {
                free(tables[i].cells[j][k]);
            }
            free(tables[i].cells[j]);
        }
        free(tables[i].cells);
    }
    free(tables);
    return 0;
}