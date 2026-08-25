// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

int main(void) {
    char **lines = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "INSERT ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *endptr;
            long idx = strtol(p, &endptr, 10);
            if (endptr != p && idx >= 0 && idx <= count) {
                while (*endptr == ' ') endptr++;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    char **new_lines = realloc(lines, capacity * sizeof(char *));
                    if (new_lines) lines = new_lines;
                }
                if (count < capacity) {
                    for (int j = count; j > idx; j--) {
                        lines[j] = lines[j - 1];
                    }
                    lines[idx] = strdup(endptr);
                    count++;
                }
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *text = line + 7;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                char **new_lines = realloc(lines, capacity * sizeof(char *));
                if (new_lines) lines = new_lines;
            }
            if (count < capacity) {
                lines[count] = strdup(text);
                count++;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *endptr;
            long idx = strtol(p, &endptr, 10);
            if (endptr != p && idx >= 0 && idx < count) {
                free(lines[idx]);
                for (int j = idx; j < count - 1; j++) {
                    lines[j] = lines[j + 1];
                }
                count--;
            }
        } else if (strncmp(line, "PATCH ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *endptr;
            long idx = strtol(p, &endptr, 10);
            if (endptr != p && idx >= 0 && idx < count) {
                while (*endptr == ' ') endptr++;
                free(lines[idx]);
                lines[idx] = strdup(endptr);
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                char *l = lines[i];
                while (*l) {
                    if (*l == ' ') putchar('_');
                    else putchar(*l);
                    l++;
                }
                putchar('\n');
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
