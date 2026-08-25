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

int main(void) {
    char **lines = NULL;
    int count = 0;
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
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "INSERT") == 0) {
            while (*p == ' ') p++;
            char *idx_s = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *text = p;
            int idx = atoi(idx_s);
            if (idx >= 0 && idx <= count) {
                lines = realloc(lines, (count + 1) * sizeof(char *));
                for (int i = count; i > idx; i--) {
                    lines[i] = lines[i - 1];
                }
                lines[idx] = strdup(text);
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            while (*p == ' ') p++;
            char *text = p;
            lines = realloc(lines, (count + 1) * sizeof(char *));
            lines[count] = strdup(text);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *idx_s = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int idx = atoi(idx_s);
            if (idx >= 0 && idx < count) {
                free(lines[idx]);
                for (int i = idx; i < count - 1; i++) {
                    lines[i] = lines[i + 1];
                }
                count--;
                if (count == 0) {
                    free(lines);
                    lines = NULL;
                } else {
                    lines = realloc(lines, count * sizeof(char *));
                }
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            while (*p == ' ') p++;
            char *idx_s = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *text = p;
            int idx = atoi(idx_s);
            if (idx >= 0 && idx < count) {
                free(lines[idx]);
                lines[idx] = strdup(text);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                for (int j = 0; lines[i][j]; j++) {
                    putchar(lines[i][j] == ' ' ? '_' : lines[i][j]);
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
