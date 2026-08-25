// F038.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *encoded;
} RLE;

int main(void) {
    RLE *items = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "STORE ", 6) == 0) {
            char *p = line + 6;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *name = p;
                char *encoded = space + 1;
                int valid = 1;
                char *t = encoded;
                while (*t) {
                    if (*t >= '0' && *t <= '9') {
                        long c_val = strtol(t, &t, 10);
                        if (c_val <= 0 || c_val > 1000 || *t == '\0') {
                            valid = 0;
                            break;
                        }
                        t++;
                    } else {
                        valid = 0;
                        break;
                    }
                }
                if (valid) {
                    RLE *tmp = realloc(items, sizeof(RLE) * (count + 1));
                    if (tmp) {
                        items = tmp;
                        items[count].name = strdup(name);
                        items[count].encoded = strdup(encoded);
                        if (items[count].name && items[count].encoded) {
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DECODE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    char *t = items[i].encoded;
                    while (*t) {
                        long cnt = strtol(t, &t, 10);
                        char c = *t;
                        for (long j = 0; j < cnt; j++) {
                            if (c == ' ') putchar('_');
                            else putchar(c);
                        }
                        t++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                free(items[idx].name);
                free(items[idx].encoded);
                for (int i = idx; i < count - 1; i++) {
                    items[i] = items[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", items[i].name);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].encoded);
    }
    free(items);
    free(line);
    return 0;
}
