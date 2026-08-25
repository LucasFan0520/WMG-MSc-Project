// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(void) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nbuf = realloc(buf, ncap);
            if (!nbuf) {
                free(buf);
                return NULL;
            }
            buf = nbuf;
            cap = ncap;
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

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char ***cells;
    int rows;
    int cols;
} CountedTable;

int main(void) {
    CountedTable *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
            while (*p == ' ') p++;
        }
        size_t clen = strlen(cmd);
        while (clen > 0 && (cmd[clen-1] == '\n' || cmd[clen-1] == '\n')) {
            cmd[clen-1] = '\0';
            clen--;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "TABLE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *rows_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *cols_s = p;
            size_t clen2 = strlen(cols_s);
            while (clen2 > 0 && (cols_s[clen2-1] == '\n' || cols_s[clen2-1] == '\n')) {
                cols_s[clen2-1] = '\0';
                clen2--;
            }
            int r = atoi(rows_s);
            int c = atoi(cols_s);
            if (r > 0 && c > 0) {
                char ***grid = malloc(r * sizeof(char**));
                for (int i = 0; i < r; i++) {
                    grid[i] = malloc(c * sizeof(char*));
                    for (int j = 0; j < c; j++) {
                        grid[i][j] = mystrdup("EMPTY");
                    }
                }
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    CountedTable *nitems = realloc(items, cap * sizeof(CountedTable));
                    if (nitems) items = nitems;
                }
                items[count].name = mystrdup(name);
                items[count].cells = grid;
                items[count].rows = r;
                items[count].cols = c;
                count++;
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *row_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *col_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *val = p;
            size_t vlen = strlen(val);
            while (vlen > 0 && (val[vlen-1] == '\n' || val[vlen-1] == '\n')) {
                val[vlen-1] = '\0';
                vlen--;
            }
            int row_idx = atoi(row_s);
            int col_idx = atoi(col_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    if (row_idx >= 0 && row_idx < items[i].rows && col_idx >= 0 && col_idx < items[i].cols) {
                        free(items[i].cells[row_idx][col_idx]);
                        items[i].cells[row_idx][col_idx] = mystrdup(val);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *row_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *col_s = p;
            size_t clen2 = strlen(col_s);
            while (clen2 > 0 && (col_s[clen2-1] == '\n' || col_s[clen2-1] == '\n')) {
                col_s[clen2-1] = '\0';
                clen2--;
            }
            int row_idx = atoi(row_s);
            int col_idx = atoi(col_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    if (row_idx >= 0 && row_idx < items[i].rows && col_idx >= 0 && col_idx < items[i].cols) {
                        printf("%s\n", items[i].cells[row_idx][col_idx]);
                    } else {
                        printf("EMPTY\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    for (int r = 0; r < items[i].rows; r++) {
                        for (int c = 0; r < items[i].cols; c++) {
                            free(items[i].cells[r][c]);
                        }
                        free(items[i].cells[r]);
                    }
                    free(items[i].cells);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
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
