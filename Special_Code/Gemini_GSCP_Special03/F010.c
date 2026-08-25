// F010.c
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
} Entry;

int main(void) {
    Entry *items = NULL;
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
        if (strcmp(cmd, "ADD") == 0) {
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
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Entry *nitems = realloc(items, cap * sizeof(Entry));
                if (nitems) items = nitems;
            }
            items[count].key = mystrdup(key);
            items[count].value = mystrdup(val);
            count++;
        } else if (strcmp(cmd, "REMOVE") == 0) {
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
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0 && strcmp(items[i].value, val) == 0) {
                    free(items[i].key);
                    free(items[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
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
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *key = p;
            size_t klen = strlen(key);
            while (klen > 0 && (key[klen-1] == '\n' || key[klen-1] == '\n')) {
                key[klen-1] = '\0';
                klen--;
            }
            int first = 1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    if (!first) putchar(' ');
                    printf("%s", items[i].value);
                    first = 0;
                }
            }
            putchar('\n');
        } else if (strcmp(cmd, "KEYS") == 0) {
            char **printed = NULL;
            size_t pcount = 0;
            for (size_t i = 0; i < count; i++) {
                int already = 0;
                for (size_t j = 0; j < pcount; j++) {
                    if (strcmp(printed[j], items[i].key) == 0) {
                        already = 1;
                        break;
                    }
                }
                if (!already) {
                    printf("%s\n", items[i].key);
                    printed = realloc(printed, (pcount + 1) * sizeof(char*));
                    printed[pcount] = items[i].key;
                    pcount++;
                }
            }
            free(printed);
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
