// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
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
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "INSERT") == 0) {
            char *idx_str = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *text = p;
            int idx = atoi(idx_str);
            if (idx >= 0 && (size_t)idx <= count) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    char **nb = realloc(lines, cap * sizeof(char *));
                    if (nb) lines = nb;
                }
                if (count < cap) {
                    for (size_t j = count; j > (size_t)idx; j--) {
                        lines[j] = lines[j - 1];
                    }
                    lines[idx] = strdup(text);
                    count++;
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                char **nb = realloc(lines, cap * sizeof(char *));
                if (nb) lines = nb;
            }
            if (count < cap) {
                lines[count] = strdup(arg1);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = atoi(arg1);
            if (idx >= 0 && (size_t)idx < count) {
                free(lines[idx]);
                for (size_t j = (size_t)idx; j < count - 1; j++) {
                    lines[j] = lines[j + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *idx_str = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *text = p;
            int idx = atoi(idx_str);
            if (idx >= 0 && (size_t)idx < count) {
                free(lines[idx]);
                lines[idx] = strdup(text);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                for (size_t j = 0; lines[i][j] != '\0'; j++) {
                    if (lines[i][j] == ' ') putchar('_');
                    else putchar(lines[i][j]);
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
