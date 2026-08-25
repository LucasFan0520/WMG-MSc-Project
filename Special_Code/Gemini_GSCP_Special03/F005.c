// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(void) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nbuf = realloc(buf, ncap);
            if (!nbuf) {
                free(buf);
                return NULL;
            }
            buf = nbuf;
            cap = ncap;
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
        char *line = read_line();
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
            while (*p == ' ') p++;
        }
        size_t clen = strlen(cmd);
        while (clen > 0 && (cmd[clen-1] == '\n' || cmd[clen-1] == '\n')) {
            cmd[clen-1] = '\0';
            clen--;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "INSERT") == 0) {
            char *idx_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *text = p;
            size_t tlen = strlen(text);
            while (tlen > 0 && (text[tlen-1] == '\n' || text[tlen-1] == '\n')) {
                text[tlen-1] = '\0';
                tlen--;
            }
            int idx = atoi(idx_s);
            if (idx >= 0 && (size_t)idx <= count) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    char **nlines = realloc(lines, cap * sizeof(char*));
                    if (nlines) lines = nlines;
                }
                for (size_t i = count; i > (size_t)idx; i--) {
                    lines[i] = lines[i-1];
                }
                lines[idx] = mystrdup(text);
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *text = p;
            size_t tlen = strlen(text);
            while (tlen > 0 && (text[tlen-1] == '\n' || text[tlen-1] == '\n')) {
                text[tlen-1] = '\0';
                tlen--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                char **nlines = realloc(lines, cap * sizeof(char*));
                if (nlines) lines = nlines;
            }
            lines[count] = mystrdup(text);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *idx_s = p;
            size_t ilen = strlen(idx_s);
            while (ilen > 0 && (idx_s[ilen-1] == '\n' || idx_s[ilen-1] == '\n')) {
                idx_s[ilen-1] = '\0';
                ilen--;
            }
            int idx = atoi(idx_s);
            if (idx >= 0 && (size_t)idx < count) {
                free(lines[idx]);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    lines[i] = lines[i+1];
                }
                count--;
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *idx_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *text = p;
            size_t tlen = strlen(text);
            while (tlen > 0 && (text[tlen-1] == '\n' || text[tlen-1] == '\n')) {
                text[tlen-1] = '\0';
                tlen--;
            }
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
