// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char ***cells;
    int rows;
    int cols;
} CountedTable;

int main() {
    CountedTable *tables = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "TABLE ", 6) == 0) {
            char name[128];
            int r, c;
            if (sscanf(line + 6, "%127s %d %d", name, &r, &c) == 3) {
                tables = realloc(tables, (count + 1) * sizeof(CountedTable));
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
        } else if (strncmp(line, "SET ", 4) == 0) {
            char name[128];
            int r, c;
            if (sscanf(line + 4, "%127s %d %d", name, &r, &c) == 3) {
                char *p = line + 4;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(tables[i].name, name) == 0) {
                        if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                            free(tables[i].cells[r][c]);
                            tables[i].cells[r][c] = strdup(p);
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char name[128];
            int r, c;
            if (sscanf(line + 4, "%127s %d %d", name, &r, &c) == 3) {
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
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
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
                    tables = realloc(tables, count * sizeof(CountedTable));
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
