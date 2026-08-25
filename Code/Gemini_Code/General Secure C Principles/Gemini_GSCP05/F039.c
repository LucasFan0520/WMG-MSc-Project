// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    char ***cells;
    int rows;
    int cols;
} Table;

int main(void) {
    Table *tables = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "TABLE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *endptr;
                long r = strtol(p, &endptr, 10);
                if (endptr != p) {
                    p = endptr;
                    while (*p == ' ') p++;
                    long c = strtol(p, &endptr, 10);
                    if (endptr != p && r > 0 && c > 0) {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            Table *new_t = realloc(tables, capacity * sizeof(Table));
                            if (new_t) tables = new_t;
                        }
                        if (count < capacity) {
                            tables[count].name = strdup(name);
                            tables[count].rows = r;
                            tables[count].cols = c;
                            tables[count].cells = malloc(r * sizeof(char **));
                            for (int i = 0; i < r; i++) {
                                tables[count].cells[i] = malloc(c * sizeof(char *));
                                for (int j = 0; j < c; j++) {
                                    tables[count].cells[i][j] = strdup("EMPTY");
                                }
                            }
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *endptr;
                long r = strtol(p, &endptr, 10);
                if (endptr != p) {
                    p = endptr;
                    while (*p == ' ') p++;
                    long c = strtol(p, &endptr, 10);
                    if (endptr != p) {
                        while (*endptr == ' ') endptr++;
                        char *val = endptr;
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
                }
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *endptr;
                long r = strtol(p, &endptr, 10);
                if (endptr != p) {
                    p = endptr;
                    while (*p == ' ') p++;
                    long c = strtol(p, &endptr, 10);
                    if (endptr != p) {
                        for (int i = 0; i < count; i++) {
                            if (strcmp(tables[i].name, name) == 0) {
                                if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                                    printf("%s\n", tables[i].cells[r][c]);
                                } else {
                                    printf("INVALID\n");
                                }
                                break;
                            }
                        }
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
                        for (int c = 0; r < tables[i].rows && c < tables[i].cols; c++) {
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
        free(line);
    }
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
