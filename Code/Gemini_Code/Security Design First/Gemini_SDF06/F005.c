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
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "INSERT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *idx_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *text = p;
            if (*idx_str == '\0') continue;
            long index = strtol(idx_str, NULL, 10);
            if (index < 0 || (size_t)index > count) continue;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                char **new_lines = realloc(lines, new_cap * sizeof(char *));
                if (!new_lines) continue;
                lines = new_lines;
                capacity = new_cap;
            }
            char *t = strdup(text);
            if (!t) continue;
            for (size_t i = count; i > (size_t)index; i--) {
                lines[i] = lines[i - 1];
            }
            lines[index] = t;
            count++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *text = p;
            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                char **new_lines = realloc(lines, new_cap * sizeof(char *));
                if (!new_lines) continue;
                lines = new_lines;
                capacity = new_cap;
            }
            char *t = strdup(text);
            if (t) {
                lines[count] = t;
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *idx_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*idx_str == '\0') continue;
            long index = strtol(idx_str, NULL, 10);
            if (index < 0 || (size_t)index >= count) continue;

            free(lines[index]);
            for (size_t i = (size_t)index; i < count - 1; i++) {
                lines[i] = lines[i + 1];
            }
            count--;
        } else if (strcmp(cmd, "PATCH") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *idx_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *text = p;
            if (*idx_str == '\0') continue;
            long index = strtol(idx_str, NULL, 10);
            if (index < 0 || (size_t)index >= count) continue;

            char *t = strdup(text);
            if (t) {
                free(lines[index]);
                lines[index] = t;
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                for (size_t j = 0; lines[i][j] != '\0'; j++) {
                    if (lines[i][j] == ' ') {
                        putchar('_');
                    } else {
                        putchar(lines[i][j]);
                    }
                }
                putchar('\n');
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
    free(line);
    return 0;
}
