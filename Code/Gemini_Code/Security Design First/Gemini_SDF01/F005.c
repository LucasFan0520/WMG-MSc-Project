// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char **lines = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "INSERT") == 0) {
            char *idx_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *text = p;

            long idx = strtol(idx_str, NULL, 10);
            if (idx >= 0 && (size_t)idx <= count) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    char **new_lines = realloc(lines, capacity * sizeof(char *));
                    if (!new_lines) exit(1);
                    lines = new_lines;
                }
                for (size_t i = count; i > (size_t)idx; i--) {
                    lines[i] = lines[i - 1];
                }
                lines[idx] = strdup(text);
                if (!lines[idx]) exit(1);
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *text = p;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                char **new_lines = realloc(lines, capacity * sizeof(char *));
                if (!new_lines) exit(1);
                lines = new_lines;
            }
            lines[count] = strdup(text);
            if (!lines[count]) exit(1);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *idx_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            long idx = strtol(idx_str, NULL, 10);
            if (idx >= 0 && (size_t)idx < count) {
                free(lines[idx]);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    lines[i] = lines[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *idx_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *text = p;

            long idx = strtol(idx_str, NULL, 10);
            if (idx >= 0 && (size_t)idx < count) {
                free(lines[idx]);
                lines[idx] = strdup(text);
                if (!lines[idx]) exit(1);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                char *tl = lines[i];
                for (size_t j = 0; tl[j] != '\0'; j++) {
                    if (tl[j] == ' ') putchar('_');
                    else putchar(tl[j]);
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
