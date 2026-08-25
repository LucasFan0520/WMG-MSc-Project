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
} Table2D;

int main(void) {
    Table2D *tables = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "TABLE ", 6) == 0) {
            char *args = line + 6;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *name = args;
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    int rows = atoi(rest);
                    int cols = atoi(sp2 + 1);
                    if (rows > 0 && cols > 0) {
                        char ***cells = calloc(rows, sizeof(char **));
                        if (cells) {
                            for (int r = 0; r < rows; r++) {
                                cells[r] = calloc(cols, sizeof(char *));
                            }
                            if (count >= capacity) {
                                capacity = capacity == 0 ? 4 : capacity * 2;
                                Table2D *next = realloc(tables, capacity * sizeof(Table2D));
                                if (!next) break;
                                tables = next;
                            }
                            tables[count].name = strdup(name);
                            tables[count].rows = rows;
                            tables[count].cols = cols;
                            tables[count].cells = cells;
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "SET ", 4) == 0) {
            char *args = line + 4;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *name = args;
                char *rest1 = sp1 + 1;
                char *sp2 = strchr(rest1, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    int row = atoi(rest1);
                    char *rest2 = sp2 + 1;
                    char *sp3 = strchr(rest2, ' ');
                    if (sp3) {
                        *sp3 = '\0';
                        int col = atoi(rest2);
                        char *val = sp3 + 1;
                        for (size_t i = 0; i < count; i++) {
                            if (strcmp(tables[i].name, name) == 0) {
                                if (row >= 0 && row < tables[i].rows && col >= 0 && col < tables[i].cols) {
                                    free(tables[i].cells[row][col]);
                                    tables[i].cells[row][col] = strdup(val);
                                }
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *args = line + 4;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *name = args;
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    int row = atoi(rest);
                    int col = atoi(sp2 + 1);
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(tables[i].name, name) == 0) {
                            if (row >= 0 && row < tables[i].rows && col >= 0 && col < tables[i].cols) {
                                if (tables[i].cells[row][col]) {
                                    printf("%s\n", tables[i].cells[row][col]);
                                } else {
                                    printf("EMPTY\n");
                                }
                            } else {
                                printf("EMPTY\n");
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    free(tables[i].name);
                    for (int r = 0; r < tables[i].rows; r++) {
                        for (int c = 0; c < tables[i].cols; c++) {
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
        } else if (strcmp(line, "TABLES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", tables[i].name);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
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
    free(line);
    return 0;
}
