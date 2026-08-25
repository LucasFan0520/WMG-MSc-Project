// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    int rows;
    int cols;
    char ***cells;
} CountedTable;

int main() {
    CountedTable *tables = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "TABLES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", tables[i].name);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "TABLE") == 0) {
            char name[256];
            int r = 0, c = 0;
            if (sscanf(p, "%255s %d %d", name, &r, &c) == 3 && r > 0 && c > 0) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    tables = realloc(tables, capacity * sizeof(CountedTable));
                }
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
        } else if (strcmp(cmd, "SET") == 0) {
            char name[256], val[256];
            int r = 0, c = 0;
            if (sscanf(p, "%255s %d %d %255s", name, &r, &c, val) == 4) {
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
        } else if (strcmp(cmd, "GET") == 0) {
            char name[256];
            int r = 0, c = 0;
            if (sscanf(p, "%255s %d %d", name, &r, &c) == 3) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(tables[i].name, name) == 0) {
                        if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                            printf("%s\n", tables[i].cells[r][c]);
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(tables[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(tables[found].name);
                    for (int r = 0; r < tables[found].rows; r++) {
                        for (int c = 0; c < tables[found].cols; c++) {
                            free(tables[found].cells[r][c]);
                        }
                        free(tables[found].cells[r]);
                    }
                    free(tables[found].cells);
                    for (int i = found; i < count - 1; i++) {
                        tables[i] = tables[i + 1];
                    }
                    count--;
                }
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
