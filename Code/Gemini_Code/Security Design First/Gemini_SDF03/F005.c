// F005.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char **arr = NULL;
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
        if (strncmp(line, "INSERT ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            char *text = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                text = p;
            }
            int idx = atoi(idx_str);
            if (idx >= 0 && (size_t)idx <= count) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    char **tmp = realloc(arr, capacity * sizeof(char *));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                for (size_t i = count; i > (size_t)idx; i--) {
                    arr[i] = arr[i - 1];
                }
                arr[idx] = strdup(text);
                if (!arr[idx]) exit(1);
                count++;
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *text = line + 7;
            while (*text == ' ') text++;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                char **tmp = realloc(arr, capacity * sizeof(char *));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count] = strdup(text);
            if (!arr[count]) exit(1);
            count++;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            int idx = atoi(p);
            if (idx >= 0 && (size_t)idx < count) {
                free(arr[idx]);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    arr[i] = arr[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "PATCH ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            char *text = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                text = p;
            }
            int idx = atoi(idx_str);
            if (idx >= 0 && (size_t)idx < count) {
                free(arr[idx]);
                arr[idx] = strdup(text);
                if (!arr[idx]) exit(1);
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                char *t = arr[i];
                while (*t) {
                    if (*t == ' ') putchar('_');
                    else putchar(*t);
                    t++;
                }
                putchar('\n');
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i]);
    }
    free(arr);
    free(line);
    return 0;
}
