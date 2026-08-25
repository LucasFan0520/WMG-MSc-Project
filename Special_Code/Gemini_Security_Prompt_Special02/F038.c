// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *encoded;
} RLEString;

int main() {
    RLEString *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
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
        while (*p == ' ') p++;
        char *name = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "STORE") == 0) {
            while (*p == ' ') p++;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                items = realloc(items, cap * sizeof(RLEString));
            }
            items[count].name = mystrdup(name);
            items[count].encoded = mystrdup(p);
            count++;
        } else if (strcmp(cmd, "DECODE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    char *ep = items[i].encoded;
                    int valid = 1;
                    while (*ep) {
                        if (*ep >= '0' && *ep <= '9') {
                            int cnt = 0;
                            while (*ep >= '0' && *ep <= '9') {
                                cnt = cnt * 10 + (*ep - '0');
                                if (cnt > 10000) {
                                    valid = 0;
                                    break;
                                }
                                ep++;
                            }
                            if (!valid || *ep == '\0') {
                                valid = 0;
                                break;
                            }
                            char c = *ep;
                            for (int k = 0; k < cnt; k++) {
                                if (c == ' ') putchar('_');
                                else putchar(c);
                            }
                            ep++;
                        } else {
                            if (*ep == ' ') putchar('_');
                            else putchar(*ep);
                            ep++;
                        }
                    }
                    if (valid) putchar('\n');
                    else printf(" INVALID\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].encoded);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", items[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].encoded);
    }
    free(items);
    return 0;
}
