// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
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
        if (strncmp(line, "INSERT ", 7) == 0) {
            char *p = line + 7;
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            char *text = "";
            if (*p == ' ') {
                *p = '\0';
                text = p + 1;
            }
            long idx = strtol(idx_str, NULL, 10);
            if (idx >= 0 && (size_t)idx <= count) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    char **nb = realloc(lines, cap * sizeof(char *));
                    if (!nb) {
                        free(line);
                        break;
                    }
                    lines = nb;
                }
                for (size_t i = count; i > (size_t)idx; i--) {
                    lines[i] = lines[i - 1];
                }
                lines[idx] = safe_dup(text);
                count++;
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *text = line + 7;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                char **nb = realloc(lines, cap * sizeof(char *));
                if (!nb) {
                    free(line);
                    break;
                }
                lines = nb;
            }
            lines[count] = safe_dup(text);
            count++;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *idx_str = line + 7;
            long idx = strtol(idx_str, NULL, 10);
            if (idx >= 0 && (size_t)idx < count) {
                free(lines[idx]);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    lines[i] = lines[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "PATCH ", 6) == 0) {
            char *p = line + 6;
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            char *text = "";
            if (*p == ' ') {
                *p = '\0';
                text = p + 1;
            }
            long idx = strtol(idx_str, NULL, 10);
            if (idx >= 0 && (size_t)idx < count) {
                free(lines[idx]);
                lines[idx] = safe_dup(text);
            }
        } else if (strcmp(line, "PRINT") == 0) {
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
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
    return 0;
}
