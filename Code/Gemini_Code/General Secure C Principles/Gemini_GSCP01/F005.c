// F005.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char **lines = NULL;
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
        if (strcmp(cmd, "INSERT") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *idx_s = p;
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
            char *text = p;
            int idx = atoi(idx_s);
            if (idx >= 0 && (size_t)idx <= count) {
                char **temp = realloc(lines, (count + 1) * sizeof(char *));
                if (temp) {
                    lines = temp;
                    for (size_t i = count; i > (size_t)idx; i--) {
                        lines[i] = lines[i - 1];
                    }
                    lines[idx] = strdup(text);
                    count++;
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *text = p;
            char **temp = realloc(lines, (count + 1) * sizeof(char *));
            if (temp) {
                lines = temp;
                lines[count] = strdup(text);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *idx_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int idx = atoi(idx_s);
            if (idx >= 0 && (size_t)idx < count) {
                free(lines[idx]);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    lines[i] = lines[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *idx_s = p;
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
            char *text = p;
            int idx = atoi(idx_s);
            if (idx >= 0 && (size_t)idx < count) {
                free(lines[idx]);
                lines[idx] = strdup(text);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                char *t = lines[i];
                while (*t) {
                    if (*t == ' ') {
                        putchar('_');
                    } else {
                        putchar(*t);
                    }
                    t++;
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
