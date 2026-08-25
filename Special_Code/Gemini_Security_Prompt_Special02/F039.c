// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char ***cells;
    int rows;
    int cols;
} CountedTable;

int main() {
    CountedTable *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *name = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "TABLE") == 0) {
            while (*p == ' ') p++;
            char *rows_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *cols_s = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int r = atoi(rows_s);
            int c = atoi(cols_s);
            if (r > 0 && c > 0) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    items = realloc(items, cap * sizeof(CountedTable));
                }
                items[count].name = mystrdup(name);
                items[count].rows = r;
                items[count].cols = c;
                items[count].cells = malloc(r * sizeof(char **));
                for (int i = 0; i < r; i++) {
                    items[count].cells[i] = malloc(c * sizeof(char *));
                    for (int j = 0; j < c; j++) {
                        items[count].cells[i][j] = mystrdup("");
                    }
                }
                count++;
            }
        } else if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ') p++;
            char *row_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *col_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *value = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int row = atoi(row_s);
            int col = atoi(col_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    if (row >= 0 && row < items[i].rows && col >= 0 && col < items[i].cols) {
                        free(items[i].cells[row][col]);
                        items[i].cells[row][col] = mystrdup(value);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ') p++;
            char *row_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *col_s = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int row = atoi(row_s);
            int col = atoi(col_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    if (row >= 0 && row < items[i].rows && col >= 0 && col < items[i].cols) {
                        if (strlen(items[i].cells[row][col]) == 0) {
                            printf("EMPTY\n");
                        } else {
                            printf("%s\n", items[i].cells[row][col]);
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    for (int r = 0; r < items[i].rows; r++) {
                        for (int c = 0; c < items[i].cols; c++) {
                            free(items[i].cells[r][c]);
                        }
                        free(items[i].cells[r]);
                    }
                    free(items[i].cells);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", items[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        for (int r = 0; r < items[i].rows; r++) {
            for (int c = 0; c < items[i].cols; c++) {
                free(items[i].cells[r][c]);
            }
            free(items[i].cells[r]);
        }
        free(items[i].cells);
    }
    free(items);
    return 0;
}
