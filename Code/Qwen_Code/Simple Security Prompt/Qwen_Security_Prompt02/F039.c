// F039.c
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
int t_count = 0;

int find_table(const char *name) {
    for (int i = 0; i < t_count; i++) {
        if (strcmp(tables[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void free_table(int idx) {
    free(tables[idx].name);
    for (int r = 0; r < tables[idx].rows; r++) {
        for (int c = 0; c < tables[idx].cols; c++) {
            if (tables[idx].cells[r][c] != NULL) {
                free(tables[idx].cells[r][c]);
            }
        }
        free(tables[idx].cells[r]);
    }
    free(tables[idx].cells);
}

int main() {
    char cmd[32];
    char name[1024];
    char value[10240];
    int rows;
    int cols;
    int row;
    int col;

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "TABLE") == 0) {
            scanf("%s %d %d", name, &rows, &cols);
            if (rows <= 0 || cols <= 0) {
                continue;
            }
            int idx = find_table(name);
            if (idx == -1) {
                tables = realloc(tables, sizeof(Table) * (t_count + 1));
                tables[t_count].name = strdup(name);
                tables[t_count].rows = rows;
                tables[t_count].cols = cols;
                tables[t_count].cells = malloc(sizeof(char **) * rows);
                for (int r = 0; r < rows; r++) {
                    tables[t_count].cells[r] = malloc(sizeof(char *) * cols);
                    for (int c = 0; c < cols; c++) {
                        tables[t_count].cells[r][c] = NULL;
                    }
                }
                t_count++;
            }
        } else if (strcmp(cmd, "SET") == 0) {
            scanf("%s %d %d %s", name, &row, &col, value);
            int idx = find_table(name);
            if (idx != -1) {
                if (row >= 0 && row < tables[idx].rows && col >= 0 && col < tables[idx].cols) {
                    if (tables[idx].cells[row][col] != NULL) {
                        free(tables[idx].cells[row][col]);
                    }
                    tables[idx].cells[row][col] = strdup(value);
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            scanf("%s %d %d", name, &row, &col);
            int idx = find_table(name);
            if (idx != -1) {
                if (row >= 0 && row < tables[idx].rows && col >= 0 && col < tables[idx].cols) {
                    if (tables[idx].cells[row][col] != NULL) {
                        printf("%s\n", tables[idx].cells[row][col]);
                    } else {
                        printf("EMPTY\n");
                    }
                } else {
                    printf("EMPTY\n");
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_table(name);
            if (idx != -1) {
                free_table(idx);
                for (int i = idx; i < t_count - 1; i++) {
                    tables[i] = tables[i + 1];
                }
                t_count--;
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (int i = 0; i < t_count; i++) {
                printf("%s\n", tables[i].name);
            }
        }
    }

    for (int i = 0; i < t_count; i++) {
        free_table(i);
    }
    free(tables);

    return 0;
}