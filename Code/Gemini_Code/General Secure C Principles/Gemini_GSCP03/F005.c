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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *text;
} Line;

int main() {
    Line *items = NULL;
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
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                char *tptr = items[i].text;
                while (*tptr) {
                    if (*tptr == ' ') {
                        putchar('_');
                    } else {
                        putchar(*tptr);
                    }
                    tptr++;
                }
                putchar('\n');
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "INSERT") == 0) {
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
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    Line *nb = realloc(items, cap * sizeof(Line));
                    if (nb) items = nb;
                }
                for (int j = (int)count; j > idx; j--) {
                    items[j] = items[j - 1];
                }
                items[idx].text = strdup(text);
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *text = p;
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                Line *nb = realloc(items, cap * sizeof(Line));
                if (nb) items = nb;
            }
            items[count].text = strdup(text);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            int idx = atoi(idx_str);
            if (idx >= 0 && idx < (int)count) {
                free(items[idx].text);
                for (size_t j = idx; j < count - 1; j++) {
                    items[j] = items[j + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
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
                free(items[idx].text);
                items[idx].text = strdup(text);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].text);
    }
    free(items);
    return 0;
}
