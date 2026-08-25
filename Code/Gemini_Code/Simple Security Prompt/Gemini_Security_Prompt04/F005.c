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
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

int main(void) {
    char **lines = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "INSERT ", 7) == 0) {
            char *p = line + 7;
            char *endptr;
            long idx = strtol(p, &endptr, 10);
            if (endptr != p && idx >= 0 && idx <= count) {
                while (*endptr == ' ') endptr++;
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    lines = realloc(lines, cap * sizeof(char *));
                }
                for (int i = count; i > idx; i--) {
                    lines[i] = lines[i - 1];
                }
                lines[idx] = strdup(endptr);
                count++;
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *text = line + 7;
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                lines = realloc(lines, cap * sizeof(char *));
            }
            lines[count] = strdup(text);
            count++;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *p = line + 7;
            char *endptr;
            long idx = strtol(p, &endptr, 10);
            if (endptr != p && idx >= 0 && idx < count) {
                free(lines[idx]);
                for (int i = idx; i < count - 1; i++) {
                    lines[i] = lines[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "PATCH ", 6) == 0) {
            char *p = line + 6;
            char *endptr;
            long idx = strtol(p, &endptr, 10);
            if (endptr != p && idx >= 0 && idx < count) {
                while (*endptr == ' ') endptr++;
                free(lines[idx]);
                lines[idx] = strdup(endptr);
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
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
    return 0;
}
