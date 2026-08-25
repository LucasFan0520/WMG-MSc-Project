// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char ***cells;
    int rows;
    int cols;
} RTable;

int main(void) {
    RTable *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "TABLE") == 0) {
            char *name = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *rows_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int rows = atoi(rows_str);
            int cols = atoi(p);
            if (rows > 0 && cols > 0 && rows < 1000 && cols < 1000) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    RTable *nb = realloc(list, cap * sizeof(RTable));
                    if (nb) list = nb;
                }
                if (count < cap) {
                    list[count].name = strdup(name);
                    list[count].rows = rows;
                    list[count].cols = cols;
                    list[count].cells = malloc(rows * sizeof(char **));
                    for (int r = 0; r < rows; r++) {
                        list[count].cells[r] = malloc(cols * sizeof(char *));
                        for (int c = 0; c < cols; c++) {
                            list[count].cells[r][c] = strdup("EMPTY");
                        }
                    }
                    count++;
                }
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char *name = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *row_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *col_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *value = p;
            int r = atoi(row_str);
            int c = atoi(col_str);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    if (r >= 0 && r < list[i].rows && c >= 0 && c < list[i].cols) {
                        free(list[i].cells[r][c]);
                        list[i].cells[r][c] = strdup(value);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *name = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *row_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int r = atoi(row_str);
            int c = atoi(p);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    if (r >= 0 && r < list[i].rows && c >= 0 && c < list[i].cols) {
                        printf("%s\n", list[i].cells[r][c]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, arg1) == 0) {
                    free(list[i].name);
                    for (int r = 0; r < list[i].rows; r++) {
                        for (int c = 0; c < list[i].cols; c++) {
                            free(list[i].cells[r][c]);
                        }
                        free(list[i].cells[r]);
                    }
                    free(list[i].cells);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", list[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        for (int r = 0; r < list[i].rows; r++) {
            for (int c = 0; c < list[i].cols; c++) {
                free(list[i].cells[r][c]);
            }
            free(list[i].cells[r]);
        }
        free(list[i].cells);
    }
    free(list);
    return 0;
}
