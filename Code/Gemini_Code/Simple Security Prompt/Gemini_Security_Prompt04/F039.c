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
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
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
    int cap = 0;
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
                char *endptr;
                long r = strtol(p + 1, &endptr, 10);
                long c = strtol(endptr, NULL, 10);
                if (r > 0 && c > 0) {
                    if (count >= cap) {
                        cap = cap == 0 ? 16 : cap * 2;
                        tables = realloc(tables, cap * sizeof(Table));
                    }
                    tables[count].name = strdup(name);
                    tables[count].rows = (int)r;
                    tables[count].cols = (int)c;
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
        } else if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *endptr;
                long r = strtol(p + 1, &endptr, 10);
                long c = strtol(endptr, &endptr, 10);
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
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *endptr;
                long r = strtol(p + 1, &endptr, 10);
                long c = strtol(endptr, NULL, 10);
                for (int i = 0; i < count; i++) {
                    if (strcmp(tables[i].name, name) == 0) {
                        if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                            printf("%s\n", tables[i].cells[r][c]);
                        } else {
                            printf("EMPTY\n");
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(tables[found].name);
                for (int i = 0; i < tables[found].rows; i++) {
                    for (int j = 0; j < tables[found].cols; j++) {
                        free(tables[found].cells[i][j]);
                    }
                    free(tables[found].cells[i]);
                }
                free(tables[found].cells);
                for (int i = found; i < count - 1; i++) {
                    tables[i] = tables[i + 1];
                }
                count--;
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
