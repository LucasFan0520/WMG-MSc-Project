// F004.c
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
    char *key;
    char *value;
} Variable;

int main(void) {
    Variable *items = NULL;
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
        if (strcmp(cmd, "SET") == 0) {
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *val = p;
            size_t vlen = strlen(val);
            while (vlen > 0 && (val[vlen-1] == '\n' || val[vlen-1] == '\n')) {
                val[vlen-1] = '\0';
                vlen--;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(items[found].value);
                items[found].value = mystrdup(val);
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    Variable *nitems = realloc(items, cap * sizeof(Variable));
                    if (nitems) items = nitems;
                }
                items[count].key = mystrdup(key);
                items[count].value = mystrdup(val);
                count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = p;
            size_t klen = strlen(key);
            while (klen > 0 && (key[klen-1] == '\n' || key[klen-1] == '\n')) {
                key[klen-1] = '\0';
                klen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].key, key) == 0) {
                    free(items[i].key);
                    free(items[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *text = p;
            size_t tlen = strlen(text);
            while (tlen > 0 && (text[tlen-1] == '\n' || text[tlen-1] == '\n')) {
                text[tlen-1] = '\0';
                tlen--;
            }
            char *src = text;
            while (*src) {
                if (*src == '{') {
                    char *end = strchr(src, '}');
                    if (end) {
                        *end = '\0';
                        char *k = src + 1;
                        char *val = NULL;
                        for (size_t i = 0; i < count; i++) {
                            if (strcmp(items[i].key, k) == 0) {
                                val = items[i].value;
                                break;
                            }
                        }
                        if (val) {
                            printf("%s", val);
                        } else {
                            printf("{%s}", k);
                        }
                        *end = '}';
                        src = end + 1;
                    } else {
                        putchar(*src);
                        src++;
                    }
                } else {
                    putchar(*src);
                    src++;
                }
            }
            putchar('\n');
        } else if (strcmp(cmd, "VARS") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].key, items[i].value);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].key);
        free(items[i].value);
    }
    free(items);
    return 0;
}
