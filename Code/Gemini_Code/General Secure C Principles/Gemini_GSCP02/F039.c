// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int rows;
    int cols;
    char ***cells;
} CountedTable;

int main(void) {
    CountedTable *tables = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "TABLE ", 6) == 0) {
            char name[256];
            int r, c;
            if (sscanf(line + 6, "%255s %d %d", name, &r, &c) == 3) {
                if (r > 0 && c > 0 && r <= 1000 && c <= 1000) {
                    char ***matrix = malloc(r * sizeof(char **));
                    if (matrix) {
                        for (int i = 0; i < r; i++) {
                            matrix[i] = calloc(c, sizeof(char *));
                        }
                        CountedTable *tmp = realloc(tables, (count + 1) * sizeof(CountedTable));
                        if (tmp) {
                            tables = tmp;
                            tables[count].name = strdup(name);
                            tables[count].rows = r;
                            tables[count].cols = c;
                            tables[count].cells = matrix;
                            count++;
                        } else {
                            for (int i = 0; i < r; i++) free(matrix[i]);
                            free(matrix);
                        }
                    }
                }
            }
        } else if (strncmp(line, "SET ", 4) == 0) {
            char name[256], val[256];
            int r, c;
            if (sscanf(line + 4, "%255s %d %d %255s", name, &r, &c, val) == 4) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tables[i].name, name) == 0) {
                        if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                            free(tables[i].cells[r][c]);
                            tables[i].cells[r][c] = strdup(val);
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char name[256];
            int r, c;
            if (sscanf(line + 4, "%255s %d %d", name, &r, &c) == 3) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tables[i].name, name) == 0) {
                        if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                            if (tables[i].cells[r][c]) {
                                printf("%s\n", tables[i].cells[r][c]);
                            } else {
                                printf("EMPTY\n");
                            }
                        } else {
                            printf("INVALID\n");
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    free(tables[i].name);
                    for (int r = 0; r < tables[i].rows; r++) {
                        for (int c = 0; c < tables[i].cols; c++) {
                            free(tables[i].cells[r][c]);
                        }
                        free(tables[i].cells[r]);
                    }
                    free(tables[i].cells);
                    for (int j = i; j < count - 1; j++) {
                        tables[j] = tables[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "TABLES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", tables[i].name);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(tables[i].name);
        for (int r = 0; r < tables[i].rows; r++) {
            for (int c = 0; c < tables[i].cols; c++) {
                free(tables[i].cells[r][c]);
            }
            free(tables[i].cells[r]);
        }
        free(tables[i].cells);
    }
    free(tables);
    return 0;
}
