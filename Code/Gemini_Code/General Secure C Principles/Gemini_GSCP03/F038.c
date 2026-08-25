// F038.c
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
    char *name;
    char *encoded;
} RLE;

int main() {
    RLE *items = NULL;
    int count = 0;
    int cap = 0;
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
        if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", items[i].name);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "STORE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *encoded = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                RLE *nb = realloc(items, cap * sizeof(RLE));
                if (nb) items = nb;
            }
            items[count].name = strdup(name);
            items[count].encoded = strdup(encoded);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].encoded);
                    for (int j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    char *e = items[i].encoded;
                    int valid = 1;
                    while (*e) {
                        if (*e >= '0' && *e <= '9') {
                            long run = 0;
                            while (*e >= '0' && *e <= '9') {
                                run = run * 10 + (*e - '0');
                                if (run > 10000) {
                                    valid = 0;
                                    break;
                                }
                                e++;
                            }
                            if (!valid || *e == '\0') {
                                valid = 0;
                                break;
                            }
                            char ch = *e;
                            for (int j = 0; j < run; j++) {
                                if (ch == ' ') putchar('_');
                                else putchar(ch);
                            }
                            e++;
                        } else {
                            valid = 0;
                            break;
                        }
                    }
                    if (valid) {
                        putchar('\n');
                    }
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].encoded);
    }
    free(items);
    return 0;
}
