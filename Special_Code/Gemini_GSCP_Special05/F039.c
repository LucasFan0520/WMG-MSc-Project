// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *name;
    long long rows;
    long long cols;
    char **cells;
} CountedTable;

int main(void) {
    CountedTable *tables = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "TABLE ", 6) == 0) {
            char *p = line + 6;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *rows_str = p;
            while (*p && *p != ' ') p++;
            char *cols_str = "";
            if (*p == ' ') {
                *p = '\0';
                cols_str = p + 1;
            }
            long long r = strtoll(rows_str, NULL, 10);
            long long c = strtoll(cols_str, NULL, 10);
            if (r > 0 && c > 0) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    CountedTable *nb = realloc(tables, cap * sizeof(CountedTable));
                    if (!nb) {
                        free(line);
                        break;
                    }
                    tables = nb;
                }
                tables[count].name = safe_dup(name);
                tables[count].rows = r;
                tables[count].cols = c;
                tables[count].cells = calloc(r * c, sizeof(char *));
                count++;
            }
        } else if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *row_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *col_str = p;
            while (*p && *p != ' ') p++;
            char *value = "";
            if (*p == ' ') {
                *p = '\0';
                value = p + 1;
            }
            long long row = strtoll(row_str, NULL, 10);
            long long col = strtoll(col_str, NULL, 10);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    if (row >= 0 && row < tables[i].rows && col >= 0 && col < tables[i].cols) {
                        size_t idx = (size_t)row * (size_t)tables[i].cols + (size_t)col;
                        free(tables[i].cells[idx]);
                        tables[i].cells[idx] = safe_dup(value);
                    }
                    break;
                }
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *p = line + 4;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *row_str = p;
            while (*p && *p != ' ') p++;
            char *col_str = "";
            if (*p == ' ') {
                *p = '\0';
                col_str = p + 1;
            }
            long long row = strtoll(row_str, NULL, 10);
            long long col = strtoll(col_str, NULL, 10);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    if (row >= 0 && row < tables[i].rows && col >= 0 && col < tables[i].cols) {
                        size_t idx = (size_t)row * (size_t)tables[i].cols + (size_t)col;
                        if (tables[i].cells[idx]) {
                            printf("%s\n", tables[i].cells[idx]);
                        } else {
                            printf("EMPTY\n");
                        }
                    } else {
                        printf("EMPTY\n");
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    free(tables[i].name);
                    for (size_t j = 0; j < (size_t)(tables[i].rows * tables[i].cols); j++) {
                        free(tables[i].cells[j]);
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
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(tables[i].name);
        for (size_t j = 0; j < (size_t)(tables[i].rows * tables[i].cols); j++) {
            free(tables[i].cells[j]);
                    }
        free(tables[i].cells);
    }
    free(tables);
    return 0;
}
