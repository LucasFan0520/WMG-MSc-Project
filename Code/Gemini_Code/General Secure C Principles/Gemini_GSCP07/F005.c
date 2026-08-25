// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
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

int main(void) {
    char **list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
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
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *text = p;
            int idx = atoi(idx_str);
            if (idx >= 0 && idx <= (int)count) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    char **nl = realloc(list, capacity * sizeof(char *));
                    if (!nl) break;
                    list = nl;
                }
                for (int i = (int)count; i > idx; i--) {
                    list[i] = list[i - 1];
                }
                list[idx] = strdup(text);
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            while (*p == ' ') p++;
            char *text = p;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                char **nl = realloc(list, capacity * sizeof(char *));
                if (!nl) break;
                list = nl;
            }
            list[count] = strdup(text);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int idx = atoi(idx_str);
            if (idx >= 0 && idx < (int)count) {
                free(list[idx]);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            while (*p == ' ') p++;
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *text = p;
            int idx = atoi(idx_str);
            if (idx >= 0 && idx < (int)count) {
                char *nt = strdup(text);
                if (nt) {
                    free(list[idx]);
                    list[idx] = nt;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                for (size_t j = 0; list[i][j]; j++) {
                    if (list[i][j] == ' ') putchar('_');
                    else putchar(list[i][j]);
                }
                putchar('\n');
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i]);
    }
    free(list);
    return 0;
}
