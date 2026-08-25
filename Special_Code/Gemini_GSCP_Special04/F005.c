/* F005.c */
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
        len--;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

int main(void) {
    char **lines = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p == ' ') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "INSERT") == 0) {
            while (*p == ' ') p++;
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *text = p;
            long idx = strtol(idx_str, NULL, 10);
            if (idx >= 0 && idx <= (long)count) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    char **nl = realloc(lines, cap * sizeof(char*));
                    if (nl) lines = nl;
                }
                for (size_t i = count; i > (size_t)idx; i--) {
                    lines[i] = lines[i - 1];
                }
                lines[idx] = mystrdup(text);
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            while (*p == ' ') p++;
            char *text = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                char **nl = realloc(lines, cap * sizeof(char*));
                if (nl) lines = nl;
            }
            lines[count] = mystrdup(text);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            long idx = strtol(idx_str, NULL, 10);
            if (idx >= 0 && idx < (long)count) {
                free(lines[idx]);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    lines[i] = lines[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            while (*p == ' ') p++;
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *text = p;
            long idx = strtol(idx_str, NULL, 10);
            if (idx >= 0 && idx < (long)count) {
                char *nt = mystrdup(text);
                if (nt) {
                    free(lines[idx]);
                    lines[idx] = nt;
                }
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
