// F039.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int rows;
    int cols;
    char ***cells;
} CountedTable;

int main(void) {
    CountedTable *list = NULL;
    size_t count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "TABLE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *r_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *c_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int rows = atoi(r_s);
            int cols = atoi(c_s);
            if (rows > 0 && cols > 0) {
                char ***cells = malloc(rows * sizeof(char **));
                if (cells) {
                    for (int i = 0; i < rows; i++) {
                        cells[i] = malloc(cols * sizeof(char *));
                        for (int j = 0; j < cols; j++) {
                            cells[i][j] = strdup("EMPTY");
                        }
                    }
                    CountedTable *temp = realloc(list, (count + 1) * sizeof(CountedTable));
                    if (temp) {
                        list = temp;
                        list[count].name = strdup(name);
                        list[count].rows = rows;
                        list[count].cols = cols;
                        list[count].cells = cells;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *r_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *c_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *val = p;
            int r_idx = atoi(r_s);
            int c_idx = atoi(c_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    if (r_idx >= 0 && r_idx < list[i].rows && c_idx >= 0 && c_idx < list[i].cols) {
                        free(list[i].cells[r_idx][c_idx]);
                        list[i].cells[r_idx][c_idx] = strdup(val);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *r_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *c_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int r_idx = atoi(r_s);
            int c_idx = atoi(c_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    if (r_idx >= 0 && r_idx < list[i].rows && c_idx >= 0 && c_idx < list[i].cols) {
                        printf("%s\n", list[i].cells[r_idx][c_idx]);
                    } else {
                        printf("EMPTY\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
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
    free(line);
    return 0;
}
