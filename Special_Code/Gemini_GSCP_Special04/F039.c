/* F039.c */
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
        len--;
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
    char ***grid;
    long long rows;
    long long cols;
} Table;

int main(void) {
    Table *tables = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p == ' ') { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "TABLE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *r_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *c_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            long long rows = strtoll(r_str, NULL, 10);
            long long cols = strtoll(c_str, NULL, 10);
            if (rows > 0 && cols > 0 && rows < 1000 && cols < 1000) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Table *nt = realloc(tables, cap * sizeof(Table));
                    if (nt) tables = nt;
                }
                char *nn = mystrdup(name);
                char ***grid = calloc(rows, sizeof(char **));
                if (grid) {
                    for (long long i = 0; i < rows; i++) {
                        grid[i] = calloc(cols, sizeof(char *));
                    }
                }
                if (nn && grid) {
                    tables[count].name = nn;
                    tables[count].grid = grid;
                    tables[count].rows = rows;
                    tables[count].cols = cols;
                    count++;
                } else {
                    free(nn);
                    if (grid) {
                        for (long long i = 0; i < rows; i++) free(grid[i]);
                        free(grid);
                    }
                }
            }
        } else if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *r_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *c_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *val = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            long long r = strtoll(r_str, NULL, 10);
            long long c = strtoll(c_str, NULL, 10);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                        char *nv = mystrdup(val);
                        if (nv) {
                            free(tables[i].grid[r][c]);
                            tables[i].grid[r][c] = nv;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *r_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *c_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            long long r = strtoll(r_str, NULL, 10);
            long long c = strtoll(c_str, NULL, 10);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols && tables[i].grid[r][c]) {
                        printf("%s\n", tables[i].grid[r][c]);
                    } else {
                        printf("EMPTY\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            size_t i = 0;
            while (i < count) {
                if (strcmp(tables[i].name, name) == 0) {
                    free(tables[i].name);
                    for (long long r = 0; r < tables[i].rows; r++) {
                        for (long long c = 0; r < tables[i].cols; c++) { 
                            free(tables[i].grid[r][c]);
                        }
                        free(tables[i].grid[r]);
                    }
                    free(tables[i].grid);
                    for (size_t j = i; j < count - 1; j++) {
                        tables[j] = tables[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", tables[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(tables[i].name);
        for (long long r = 0; r < tables[i].rows; r++) {
            for (long long c = 0; c < tables[i].cols; c++) {
                free(tables[i].grid[r][c]);
            }
            free(tables[i].grid[r]);
        }
        free(tables[i].grid);
    }
    free(tables);
    return 0;
}
