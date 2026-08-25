// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

int main(void) {
    char **lines = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "INSERT") == 0) {
            char *idx_s = p;
            while (*idx_s == ' ') idx_s++;
            char *idx_end = idx_s;
            while (*idx_end && *idx_end != ' ') idx_end++;
            char *text = idx_end;
            if (*idx_end != '\0') {
                *idx_end = '\0';
                text++;
            }
            while (*text == ' ') text++;
            int idx = atoi(idx_s);
            if (idx >= 0 && (size_t)idx <= count) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    char **new_lines = realloc(lines, capacity * sizeof(char *));
                    if (!new_lines) {
                        free(line);
                        break;
                    }
                    lines = new_lines;
                }
                for (size_t i = count; i > (size_t)idx; i--) {
                    lines[i] = lines[i - 1];
                }
                lines[idx] = mystrdup(text);
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *text = p;
            while (*text == ' ') text++;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                char **new_lines = realloc(lines, capacity * sizeof(char *));
                if (!new_lines) {
                    free(line);
                    break;
                }
                lines = new_lines;
            }
            lines[count] = mystrdup(text);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *idx_s = p;
            while (*idx_s == ' ') idx_s++;
            char *idx_end = idx_s;
            while (*idx_end && *idx_end != ' ') idx_end++;
            *idx_end = '\0';
            int idx = atoi(idx_s);
            if (idx >= 0 && (size_t)idx < count) {
                free(lines[idx]);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    lines[i] = lines[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *idx_s = p;
            while (*idx_s == ' ') idx_s++;
            char *idx_end = idx_s;
            while (*idx_end && *idx_end != ' ') idx_end++;
            char *text = idx_end;
            if (*idx_end != '\0') {
                *idx_end = '\0';
                text++;
            }
            while (*text == ' ') text++;
            int idx = atoi(idx_s);
            if (idx >= 0 && (size_t)idx < count) {
                free(lines[idx]);
                lines[idx] = mystrdup(text);
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
