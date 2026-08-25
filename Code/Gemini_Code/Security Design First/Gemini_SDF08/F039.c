// F039.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char **cells;
    int rows;
    int cols;
} Table;

int main(void) {
    Table *tables = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "TABLE ", 6) == 0) {
            char *p = line + 6;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *name = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    int r = atoi(p2);
                    int c = atoi(s2 + 1);
                    if (r > 0 && c > 0) {
                        Table *tmp = realloc(tables, sizeof(Table) * (count + 1));
                        if (tmp) {
                            tables = tmp;
                            tables[count].name = strdup(name);
                            tables[count].rows = r;
                            tables[count].cols = c;
                            tables[count].cells = calloc(r * c, sizeof(char *));
                            if (tables[count].name && tables[count].cells) {
                                count++;
                            } else {
                                free(tables[count].name);
                                free(tables[count].cells);
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *name = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    int r = atoi(p2);
                    char *p3 = s2 + 1;
                    char *s3 = strchr(p3, ' ');
                    if (s3) {
                        *s3 = '\0';
                        int c = atoi(p3);
                        char *val = s3 + 1;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(tables[i].name, name) == 0) {
                                if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                                    char *nv = strdup(val);
                                    if (nv) {
                                        free(tables[i].cells[r * tables[i].cols + c]);
                                        tables[i].cells[r * tables[i].cols + c] = nv;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *p = line + 4;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *name = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    int r = atoi(p2);
                    int c = atoi(s2 + 1);
                    for (int i = 0; i < count; i++) {
                        if (strcmp(tables[i].name, name) == 0) {
                            if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                                char *cell = tables[i].cells[r * tables[i].cols + c];
                                if (cell) printf("%s\n", cell);
                                else printf("EMPTY\n");
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                free(tables[idx].name);
                for (int j = 0; j < tables[idx].rows * tables[idx].cols; j++) {
                    free(tables[idx].cells[j]);
                }
                free(tables[idx].cells);
                for (int i = idx; i < count - 1; i++) {
                    tables[i] = tables[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "TABLES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", tables[i].name);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(tables[i].name);
        for (int j = 0; j < tables[i].rows * tables[i].cols; j++) {
            free(tables[i].cells[j]);
        }
        free(tables[i].cells);
    }
    free(tables);
    free(line);
    return 0;
}
