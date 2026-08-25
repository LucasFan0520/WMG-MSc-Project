// F011.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

int main(void) {
    Draft *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    Draft disc = {NULL, NULL};
    int has_disc = 0;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "CREATE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            char *body = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                body = p;
            }
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Draft *tmp = realloc(arr, capacity * sizeof(Draft));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].title = strdup(title);
                arr[count].body = strdup(body);
                if (!arr[count].title || !arr[count].body) exit(1);
                count++;
            }
        } else if (strncmp(line, "REVISE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            char *body = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                body = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    free(arr[i].body);
                    arr[i].body = strdup(body);
                    if (!arr[i].body) exit(1);
                    break;
                }
            }
        } else if (strncmp(line, "DISCARD ", 8) == 0) {
            char *title = line + 8;
            while (*title == ' ') title++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    if (has_disc) {
                        free(disc.title);
                        free(disc.body);
                    }
                    disc.title = arr[i].title;
                    disc.body = arr[i].body;
                    has_disc = 1;
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "RESTORE") == 0) {
            if (has_disc) {
                int found = 0;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, disc.title) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Draft *tmp = realloc(arr, capacity * sizeof(Draft));
                        if (!tmp) exit(1);
                        arr = tmp;
                    }
                    arr[count].title = disc.title;
                    arr[count].body = disc.body;
                    count++;
                } else {
                    free(disc.title);
                    free(disc.body);
                }
                has_disc = 0;
                disc.title = NULL;
                disc.body = NULL;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') title++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    printf("%s\n", arr[i].body);
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].title, arr[i].body);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].title);
        free(arr[i].body);
    }
    free(arr);
    if (has_disc) {
        free(disc.title);
        free(disc.body);
    }
    free(line);
    return 0;
}
