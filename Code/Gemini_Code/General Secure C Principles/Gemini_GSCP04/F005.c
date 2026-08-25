// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

int main(void) {
    char **lines = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "INSERT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *idx_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *text = p;
            long long index = atoll(idx_s);
            if (index >= 0 && (size_t)index <= count) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    char **new_lines = realloc(lines, capacity * sizeof(char *));
                    if (new_lines) lines = new_lines;
                }
                if (count < capacity) {
                    for (size_t i = count; i > (size_t)index; i--) {
                        lines[i] = lines[i - 1];
                    }
                    lines[index] = strdup(text);
                    count++;
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *text = p;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                char **new_lines = realloc(lines, capacity * sizeof(char *));
                if (new_lines) lines = new_lines;
            }
            if (count < capacity) {
                lines[count] = strdup(text);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *idx_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long index = atoll(idx_s);
            if (index >= 0 && (size_t)index < count) {
                free(lines[index]);
                for (size_t i = (size_t)index; i < count - 1; i++) {
                    lines[i] = lines[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *idx_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *text = p;
            long long index = atoll(idx_s);
            if (index >= 0 && (size_t)index < count) {
                free(lines[index]);
                lines[index] = strdup(text);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                char *t = lines[i];
                while (*t) {
                    if (*t == ' ') putchar('_');
                    else putchar(*t);
                    t++;
                }
                putchar('\n');
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
