// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    int rows;
    int cols;
    char ***cells;
} Table;

int main() {
    Table *tables = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "TABLES") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", tables[i].name);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "TABLE") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = args;
                char *rows_str = p2 + 1;
                char *p3 = strchr(rows_str, ' ');
                if (p3) {
                    *p3 = '\0';
                    int r = atoi(rows_str);
                    int c = atoi(p3 + 1);
                    if (r > 0 && c > 0) {
                        tables = realloc(tables, sizeof(Table) * (count + 1));
                        tables[count].name = strdup(name);
                        tables[count].rows = r;
                        tables[count].cols = c;
                        tables[count].cells = malloc(sizeof(char**) * r);
                        for (int i = 0; i < r; i++) {
                            tables[count].cells[i] = calloc(c, sizeof(char*));
                        }
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = args;
                char *row_str = p2 + 1;
                char *p3 = strchr(row_str, ' ');
                if (p3) {
                    *p3 = '\0';
                    int r = atoi(row_str);
                    char *col_str = p3 + 1;
                    char *p4 = strchr(col_str, ' ');
                    if (p4) {
                        *p4 = '\0';
                        int c = atoi(col_str);
                        char *val = p4 + 1;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(tables[i].name, name) == 0) {
                                if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                                    if (tables[i].cells[r][c]) free(tables[i].cells[r][c]);
                                    tables[i].cells[r][c] = strdup(val);
                                }
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = args;
                char *row_str = p2 + 1;
                char *p3 = strchr(row_str, ' ');
                if (p3) {
                    *p3 = '\0';
                    int r = atoi(row_str);
                    int c = atoi(p3 + 1);
                    for (int i = 0; i < count; i++) {
                        if (strcmp(tables[i].name, name) == 0) {
                            if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols && tables[i].cells[r][c]) {
                                printf("%s\n", tables[i].cells[r][c]);
                            } else {
                                printf("EMPTY\n");
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(tables[i].name, args) == 0) {
                    free(tables[i].name);
                    for (int r = 0; r < tables[i].rows; r++) {
                        for (int c = 0; c < tables[i].cols; c++) {
                            if (tables[i].cells[r][c]) free(tables[i].cells[r][c]);
                        }
                        free(tables[i].cells[r]);
                    }
                    free(tables[i].cells);
                    for (int j = i; j < count - 1; j++) {
                        tables[j] = tables[j + 1];
                    }
                    count--;
                    if (count == 0) { free(tables); tables = NULL; }
                    else { tables = realloc(tables, sizeof(Table) * count); }
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(tables[i].name);
        for (int r = 0; r < tables[i].rows; r++) {
            for (int c = 0; c < tables[i].cols; c++) {
                if (tables[i].cells[r][c]) free(tables[i].cells[r][c]);
            }
            free(tables[i].cells[r]);
        }
        free(tables[i].cells);
    }
    free(tables);
    return 0;
}
