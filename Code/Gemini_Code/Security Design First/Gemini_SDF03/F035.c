// F035.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *payload;
} Job;

int main(void) {
    Job *arr = NULL;
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
        if (strncmp(line, "ENQUEUE ", 8) == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *payload = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                payload = p;
            }
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Job *tmp = realloc(arr, capacity * sizeof(Job));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].name = strdup(name);
            arr[count].payload = strdup(payload);
            if (!arr[count].name || !arr[count].payload) exit(1);
            count++;
        } else if (strncmp(line, "CLONE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *oldName = p;
            while (*p && *p != ' ') p++;
            char *newName = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                newName = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, oldName) == 0) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Job *tmp = realloc(arr, capacity * sizeof(Job));
                        if (!tmp) exit(1);
                        arr = tmp;
                    }
                    arr[count].name = strdup(newName);
                    arr[count].payload = strdup(arr[i].payload);
                    if (!arr[count].name || !arr[count].payload) exit(1);
                    count++;
                    break;
                }
            }
        } else if (strncmp(line, "CANCEL ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; ) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].payload);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "RUN ", 4) == 0) {
            char *name = line + 4;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s\n", arr[i].payload);
                    free(arr[i].name);
                    free(arr[i].payload);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].payload);
    }
    free(arr);
    free(line);
    return 0;
}
