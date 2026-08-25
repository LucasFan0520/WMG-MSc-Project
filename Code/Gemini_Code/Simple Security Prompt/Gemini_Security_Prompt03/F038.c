// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *encoded;
} RLEData;

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
    RLEData *list = NULL;
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
        if (strcmp(cmd, "STORE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *enc = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            list = realloc(list, (count + 1) * sizeof(RLEData));
            list[count].name = strdup(name);
            list[count].encoded = strdup(enc);
            count++;
        } else if (strcmp(cmd, "DECODE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].name && strcmp(list[i].name, name) == 0) {
                    char *s = list[i].encoded;
                    int valid = 1;
                    while (*s && valid) {
                        int num = 0;
                        if (*s >= '0' && *s <= '9') {
                            while (*s >= '0' && *s <= '9') {
                                num = num * 10 + (*s - '0');
                                if (num > 10000) {
                                    valid = 0;
                                    break;
                                }
                                s++;
                            }
                        } else {
                            valid = 0;
                            break;
                        }
                        if (!valid || num <= 0) break;
                        char ch = *s;
                        if (!ch) break;
                        s++;
                        for (int k = 0; k < num; k++) {
                            putchar(ch == ' ' ? '_' : ch);
                        }
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].name && strcmp(list[i].name, name) == 0) {
                    free(list[i].name);
                    free(list[i].encoded);
                    list[i].name = NULL;
                    list[i].encoded = NULL;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                if (list[i].name) {
                    printf("%s\n", list[i].name);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (list[i].name) {
            free(list[i].name);
            free(list[i].encoded);
        }
    }
    free(list);
    return 0;
}
