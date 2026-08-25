// F028.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} RouteStop;

int main(void) {
    RouteStop *arr = NULL;
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
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *stop = p;
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                note = p;
            }
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                RouteStop *tmp = realloc(arr, capacity * sizeof(RouteStop));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].stop = strdup(stop);
            arr[count].note = strdup(note);
            if (!arr[count].stop || !arr[count].note) exit(1);
            count++;
        } else if (strncmp(line, "INSERT ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            char *stop = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                stop = p;
            }
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                note = p;
            }
            int idx = atoi(idx_str);
            if (idx >= 0 && (size_t)idx <= count) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    RouteStop *tmp = realloc(arr, capacity * sizeof(RouteStop));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                for (size_t i = count; i > (size_t)idx; i--) {
                    arr[i] = arr[i - 1];
                }
                arr[idx].stop = strdup(stop);
                arr[idx].note = strdup(note);
                if (!arr[idx].stop || !arr[idx].note) exit(1);
                count++;
            }
        } else if (strncmp(line, "MOVE ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *from_str = p;
            while (*p && *p != ' ') p++;
            char *to_str = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                to_str = p;
            }
            int from_idx = atoi(from_str);
            int to_idx = atoi(to_str);
            if (from_idx >= 0 && (size_t)from_idx < count && to_idx >= 0 && (size_t)to_idx < count && from_idx != to_idx) {
                RouteStop target = arr[from_idx];
                if (from_idx < to_idx) {
                    for (size_t i = (size_t)from_idx; i < (size_t)to_idx; i++) {
                        arr[i] = arr[i + 1];
                    }
                } else {
                    for (size_t i = (size_t)from_idx; i > (size_t)to_idx; i--) {
                        arr[i] = arr[i - 1];
                    }
                }
                arr[to_idx] = target;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            int idx = atoi(p);
            if (idx >= 0 && (size_t)idx < count) {
                free(arr[idx].stop);
                free(arr[idx].note);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    arr[i] = arr[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *stop = line + 5;
            while (*stop == ' ') stop++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].stop, stop) == 0) {
                    printf("%s\n", arr[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%zu: %s (%s)\n", i, arr[i].stop, arr[i].note);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].stop);
        free(arr[i].note);
    }
    free(arr);
    free(line);
    return 0;
}
