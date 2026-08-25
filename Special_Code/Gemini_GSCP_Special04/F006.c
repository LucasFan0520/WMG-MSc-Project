/* F006.c */
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

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    long long cnt;
} Item;

int main(void) {
    Item *inv = NULL;
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
        if (*p == ' ') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *cnt_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            long long val = strtoll(cnt_str, NULL, 10);
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].name, item) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                inv[found].cnt += val;
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Item *ni = realloc(inv, cap * sizeof(Item));
                    if (ni) inv = ni;
                }
                char *ni_name = mystrdup(item);
                if (ni_name) {
                    inv[count].name = ni_name;
                    inv[count].cnt = val;
                    count++;
                }
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            while (*p == ' ') p++;
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *cnt_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            long long val = strtoll(cnt_str, NULL, 10);
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].name, item) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                inv[found].cnt -= val;
                if (inv[found].cnt < 0) inv[found].cnt = 0;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].name, item) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(inv[found].name);
                for (size_t i = (size_t)found; i < count - 1; i++) {
                    inv[i] = inv[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ') p++;
            char *item = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            long long current_cnt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].name, item) == 0) {
                    current_cnt = inv[i].cnt;
                    break;
                }
            }
            printf("%lld\n", current_cnt);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %lld\n", inv[i].name, inv[i].cnt);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(inv[i].name);
    }
    free(inv);
    return 0;
}
