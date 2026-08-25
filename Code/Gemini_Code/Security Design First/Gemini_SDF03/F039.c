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
} StringTable;

int main(void) {
    StringTable *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "TABLE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *r_str = "0";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                r_str = p;
            }
            while (*p && *p != ' ') p++;
            char *c_str = "0";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                c_str = p;
            }
            int rows = atoi(r_str);
            int cols = atoi(c_str);
            if (rows > 0 && cols > 0) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    StringTable *tmp = realloc(arr, capacity * sizeof(StringTable));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].name = strdup(name);
                arr[count].rows = rows;
                arr[count].cols = cols;
                arr[count].cells = calloc(rows, sizeof(char **));
                if (!arr[count].name || !arr[count].cells) exit(1);
                for (int i = 0; i < rows; i++) {
                    arr[count].cells[i] = calloc(cols, sizeof(char *));
                    if (!arr[count].cells[i]) exit(1);
                }
                count++;
            }
        } else if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *r_str = "0";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                r_str = p;
            }
            while (*p && *p != ' ') p++;
            char *c_str = "0";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                c_str = p;
            }
            while (*p && *p != ' ') p++;
            char *val = p;
            int r_idx = atoi(r_str);
            int c_idx = atoi(c_str);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    if (r_idx >= 0 && r_idx < arr[i].rows && c_idx >= 0 && c_idx < arr[i].cols) {
                        free(arr[i].cells[r_idx][c_idx]);
                        arr[i].cells[r_idx][c_idx] = strdup(val);
                        if (!arr[i].cells[r_idx][c_idx]) exit(1);
                    }
                    break;
                }
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *r_str = "0";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                r_str = p;
            }
            while (*p && *p != ' ') p++;
            char *c_str = "0";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                c_str = p;
            }
            int r_idx = atoi(r_str);
            int c_idx = atoi(c_str);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    if (r_idx >= 0 && r_idx < arr[i].rows && c_idx >= 0 && c_idx < arr[i].cols) {
                        if (arr[i].cells[r_idx][c_idx]) {
                            printf("%s\n", arr[i].cells[r_idx][c_idx]);
                        } else {
                            printf("EMPTY\n");
                        }
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    for (int r = 0; r < arr[i].rows; r++) {
                        for (int c = 0; c < arr[i].cols; c++) {
                            free(arr[i].cells[r][c]);
                        }
                        free(arr[i].cells[r]);
                    }
                    free(arr[i].cells);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "TABLES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
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
    free(line);
    return 0;
}
