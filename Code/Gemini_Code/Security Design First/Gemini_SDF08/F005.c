// F005.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char **lines = NULL;
    int count = 0;
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
            char *space1 = strchr(p, ' ');
            if (space1) {
                *space1 = '\0';
                char *idx_str = p;
                char *text = space1 + 1;
                char *endptr;
                long idx = strtol(idx_str, &endptr, 10);
                if (*endptr == '\0' && idx >= 0 && idx <= count) {
                    char **tmp = realloc(lines, sizeof(char *) * (count + 1));
                    if (tmp) {
                        lines = tmp;
                        for (int i = count; i > idx; i--) {
                            lines[i] = lines[i - 1];
                        }
                        lines[idx] = strdup(text);
                        if (lines[idx]) {
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *text = line + 7;
            char **tmp = realloc(lines, sizeof(char *) * (count + 1));
            if (tmp) {
                lines = tmp;
                lines[count] = strdup(text);
                if (lines[count]) {
                    count++;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *idx_str = line + 7;
            char *endptr;
            long idx = strtol(idx_str, &endptr, 10);
            if (*endptr == '\0' && idx >= 0 && idx < count) {
                free(lines[idx]);
                for (int i = idx; i < count - 1; i++) {
                    lines[i] = lines[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "PATCH ", 6) == 0) {
            char *p = line + 6;
            char *space1 = strchr(p, ' ');
            if (space1) {
                *space1 = '\0';
                char *idx_str = p;
                char *text = space1 + 1;
                char *endptr;
                long idx = strtol(idx_str, &endptr, 10);
                if (*endptr == '\0' && idx >= 0 && idx < count) {
                    char *nt = strdup(text);
                    if (nt) {
                        free(lines[idx]);
                        lines[idx] = nt;
                    }
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                char *t = lines[i];
                while (*t) {
                    if (*t == ' ') putchar('_');
                    else putchar(*t);
                    t++;
                }
                putchar('\n');
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
    free(line);
    return 0;
}
