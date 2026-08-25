// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

void print_underscores(const char *s) {
    while (*s) {
        if (*s == ' ') {
            putchar('_');
        } else {
            putchar(*s);
        }
        s++;
    }
    putchar('\n');
}

int main() {
    char **lines = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "INSERT ", 7) == 0) {
            char *p = line + 7;
            char *idx_s = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *text = p + 1;
                int idx = atoi(idx_s);
                if (idx >= 0 && idx <= count) {
                    lines = realloc(lines, (count + 1) * sizeof(char *));
                    for (int i = count; i > idx; i--) {
                        lines[i] = lines[i - 1];
                    }
                    lines[idx] = strdup(text);
                    count++;
                }
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *text = line + 7;
            lines = realloc(lines, (count + 1) * sizeof(char *));
            lines[count] = strdup(text);
            count++;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            int idx = atoi(line + 7);
            if (idx >= 0 && idx < count) {
                free(lines[idx]);
                for (int i = idx; i < count - 1; i++) {
                    lines[i] = lines[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "PATCH ", 6) == 0) {
            char *p = line + 6;
            char *idx_s = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *text = p + 1;
                int idx = atoi(idx_s);
                if (idx >= 0 && idx < count) {
                    free(lines[idx]);
                    lines[idx] = strdup(text);
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                print_underscores(lines[i]);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
    return 0;
}
