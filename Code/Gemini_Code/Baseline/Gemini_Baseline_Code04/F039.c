// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int rows;
    int cols;
    char ***cells;
} Table;

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
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

int main() {
    Table *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
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
                char *r_s = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    int rows = atoi(r_s);
                    int cols = atoi(p + 1);
                    if (rows > 0 && cols > 0) {
                        arr = realloc(arr, (count + 1) * sizeof(Table));
                        arr[count].name = strdup(name);
                        arr[count].rows = rows;
                        arr[count].cols = cols;
                        arr[count].cells = malloc(rows * sizeof(char **));
                        for (int i = 0; i < rows; i++) {
                            arr[count].cells[i] = malloc(cols * sizeof(char *));
                            for (int j = 0; j < cols; j++) {
                                arr[count].cells[i][j] = strdup("EMPTY");
                            }
                        }
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *r_s = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    p++;
                    char *c_s = p;
                    while (*p && *p != ' ') p++;
                    if (*p == ' ') {
                        *p = '\0';
                        int r = atoi(r_s);
                        int c = atoi(c_s);
                        char *val = p + 1;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(arr[i].name, name) == 0) {
                                if (r >= 0 && r < arr[i].rows && c >= 0 && c < arr[i].cols) {
                                    free(arr[i].cells[r][c]);
                                    arr[i].cells[r][c] = strdup(val);
                                }
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *p = line + 4;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *r_s = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    int r = atoi(r_s);
                    int c = atoi(p + 1);
                    for (int i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, name) == 0) {
                            if (r >= 0 && r < arr[i].rows && c >= 0 && c < arr[i].cols) {
                                printf("%s\n", arr[i].cells[r][c]);
                            } else {
                                printf("EMPTY\n");
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    for (int r = 0; r < arr[i].rows; r++) {
                        for (int c = 0; c < arr[i].cols; c++) {
                            free(arr[i].cells[r][c]);
                        }
                        free(arr[i].cells[r]);
                    }
                    free(arr[i].cells);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "TABLES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        for (int r = 0; r < arr[i].rows; r++) {
            for (int c = 0; c < arr[i].cols; c++) {
                free(arr[i].cells[r][c]);
            }
            free(arr[i].cells[r]);
        }
        free(arr[i].cells);
    }
    free(arr);
    return 0;
}
