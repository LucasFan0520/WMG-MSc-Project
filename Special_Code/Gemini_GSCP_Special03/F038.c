// F038.c
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

typedef struct {
    char *name;
    char *encoded;
} RLEString;

int main(void) {
    RLEString *items = NULL;
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
        if (strcmp(cmd, "STORE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *enc = p;
            size_t elen = strlen(enc);
            while (elen > 0 && (enc[elen-1] == '\n' || enc[elen-1] == '\n')) {
                enc[elen-1] = '\0';
                elen--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                RLEString *nitems = realloc(items, cap * sizeof(RLEString));
                if (nitems) items = nitems;
            }
            items[count].name = mystrdup(name);
            items[count].encoded = mystrdup(enc);
            count++;
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    char *src = items[i].encoded;
                    int valid = 1;
                    while (*src && valid) {
                        if (*src >= '0' && *src <= '9') {
                            long long repeat = 0;
                            while (*src >= '0' && *src <= '9') {
                                repeat = repeat * 10 + (*src - '0');
                                if (repeat > 100000) {
                                    valid = 0;
                                    break;
                                }
                                src++;
                            }
                            if (valid && *src) {
                                char ch = *src;
                                if (ch == ' ') ch = '_';
                                for (long long r = 0; r < repeat; r++) {
                                    putchar(ch);
                                }
                                src++;
                            } else {
                                valid = 0;
                            }
                        } else {
                            valid = 0;
                        }
                    }
                    if (!valid) {
                        printf("INVALID");
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].encoded);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
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
