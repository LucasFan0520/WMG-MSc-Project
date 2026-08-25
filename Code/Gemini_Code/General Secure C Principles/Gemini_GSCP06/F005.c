// F005.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char **lines = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "INSERT ", 7) == 0) {
            char *args = line + 7;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                int idx = atoi(args);
                char *text = space + 1;
                if (idx >= 0 && (size_t)idx <= count) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        char **next = realloc(lines, capacity * sizeof(char *));
                        if (!next) break;
                        lines = next;
                    }
                    for (size_t j = count; j > (size_t)idx; j--) {
                        lines[j] = lines[j - 1];
                    }
                    lines[idx] = strdup(text);
                    count++;
                }
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *text = line + 7;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                char **next = realloc(lines, capacity * sizeof(char *));
                if (!next) break;
                lines = next;
            }
            lines[count] = strdup(text);
            count++;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            int idx = atoi(line + 7);
            if (idx >= 0 && (size_t)idx < count) {
                free(lines[idx]);
                for (size_t j = (size_t)idx; j < count - 1; j++) {
                    lines[j] = lines[j + 1];
                }
                count--;
            }
        } else if (strncmp(line, "PATCH ", 6) == 0) {
            char *args = line + 6;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                int idx = atoi(args);
                char *text = space + 1;
                if (idx >= 0 && (size_t)idx < count) {
                    free(lines[idx]);
                    lines[idx] = strdup(text);
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                char *t = lines[i];
                for (size_t k = 0; t[k] != '\0'; k++) {
                    if (t[k] == ' ') putchar('_');
                    else putchar(t[k]);
                }
                putchar('\n');
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
    return 0;
}
