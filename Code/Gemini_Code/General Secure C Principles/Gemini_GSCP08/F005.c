// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    char **lines = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "PRINT") == 0) {
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
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "APPEND") == 0) {
            if (count >= cap) {
                size_t ncap = cap == 0 ? 4 : cap * 2;
                char **nlines = realloc(lines, ncap * sizeof(char *));
                if (nlines) {
                    lines = nlines;
                    cap = ncap;
                }
            }
            if (count < cap) {
                lines[count] = strdup(args);
                count++;
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *idx_str = args;
            char *p2 = strchr(idx_str, ' ');
            if (p2) {
                *p2 = '\0';
                char *text = p2 + 1;
                long idx = strtol(idx_str, NULL, 10);
                if (idx >= 0 && idx <= (long)count) {
                    if (count >= cap) {
                        size_t ncap = cap == 0 ? 4 : cap * 2;
                        char **nlines = realloc(lines, ncap * sizeof(char *));
                        if (nlines) {
                            lines = nlines;
                            cap = ncap;
                        }
                    }
                    if (count < cap) {
                        for (size_t i = count; i > (size_t)idx; i--) {
                            lines[i] = lines[i - 1];
                        }
                        lines[idx] = strdup(text);
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            long idx = strtol(args, NULL, 10);
            if (idx >= 0 && idx < (long)count) {
                free(lines[idx]);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    lines[i] = lines[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *idx_str = args;
            char *p2 = strchr(idx_str, ' ');
            if (p2) {
                *p2 = '\0';
                char *text = p2 + 1;
                long idx = strtol(idx_str, NULL, 10);
                if (idx >= 0 && idx < (long)count) {
                    char *ntext = strdup(text);
                    if (ntext) {
                        free(lines[idx]);
                        lines[idx] = ntext;
                    }
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
    return 0;
}
