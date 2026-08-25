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
    if (c == EOF && len == 0) {
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
} Table;

int main() {
    Table *tables = NULL;
    int count = 0;
    int cap = 0;
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
        if (*p) {
            *p = '\0';
            p++;
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
        while (*p == ' ') p++;
        if (strcmp(cmd, "TABLE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *r_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *c_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int rows = atoi(r_str);
            int cols = atoi(c_str);
            if (rows > 0 && cols > 0) {
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    Table *nb = realloc(tables, cap * sizeof(Table));
                    if (nb) tables = nb;
                }
                tables[count].name = strdup(name);
                tables[count].rows = rows;
                tables[count].cols = cols;
                tables[count].cells = malloc(rows * sizeof(char **));
                for (int i = 0; i < rows; i++) {
                    tables[count].cells[i] = malloc(cols * sizeof(char *));
                    for (int j = 0; j < cols; j++) {
                        tables[count].cells[i][j] = NULL;
                    }
                }
                count++;
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *r_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *c_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *value = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int r = atoi(r_str);
            int c = atoi(c_str);
            for (int i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                        if (tables[i].cells[r][c]) free(tables[i].cells[r][c]);
                        tables[i].cells[r][c] = strdup(value);
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
            }
            while (*p == ' ') p++;
            char *r_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *c_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int r = atoi(r_str);
            int c = atoi(c_str);
            for (int i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                        if (tables[i].cells[r][c]) {
                            printf("%s\n", tables[i].cells[r][c]);
                        } else {
                            printf("EMPTY\n");
                        }
                    } else {
                        printf("EMPTY\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
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
