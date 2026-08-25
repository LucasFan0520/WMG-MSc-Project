// F006.c
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
    char *item;
    long long count;
} InvItem;

int main(void) {
    InvItem *items = NULL;
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
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *cnt_s = p;
            size_t cnlen = strlen(cnt_s);
            while (cnlen > 0 && (cnt_s[cnlen-1] == '\n' || cnt_s[cnlen-1] == '\n')) {
                cnt_s[cnlen-1] = '\0';
                cnlen--;
            }
            long long cval = atoll(cnt_s);
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].item, item) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                items[found].count += cval;
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    InvItem *nitems = realloc(items, cap * sizeof(InvItem));
                    if (nitems) items = nitems;
                }
                items[count].item = mystrdup(item);
                items[count].count = cval;
                count++;
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *cnt_s = p;
            size_t cnlen = strlen(cnt_s);
            while (cnlen > 0 && (cnt_s[cnlen-1] == '\n' || cnt_s[cnlen-1] == '\n')) {
                cnt_s[cnlen-1] = '\0';
                cnlen--;
            }
            long long cval = atoll(cnt_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].item, item) == 0) {
                    items[i].count -= cval;
                    if (items[i].count < 0) items[i].count = 0;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *item = p;
            size_t ilen = strlen(item);
            while (ilen > 0 && (item[ilen-1] == '\n' || item[ilen-1] == '\n')) {
                item[ilen-1] = '\0';
                ilen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].item, item) == 0) {
                    free(items[i].item);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *item = p;
            size_t ilen = strlen(item);
            while (ilen > 0 && (item[ilen-1] == '\n' || item[ilen-1] == '\n')) {
                item[ilen-1] = '\0';
                ilen--;
            }
            long long cval = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].item, item) == 0) {
                    cval = items[i].count;
                    break;
                }
            }
            printf("%lld\n", cval);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %lld\n", items[i].item, items[i].count);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].item);
    }
    free(items);
    return 0;
}
