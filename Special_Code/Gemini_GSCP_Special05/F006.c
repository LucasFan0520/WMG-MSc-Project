// F006.c
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
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *item;
    long long count;
} InvItem;

int main(void) {
    InvItem *inv = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *item = p;
            while (*p && *p != ' ') p++;
            char *cnt_str = "0";
            if (*p == ' ') {
                *p = '\0';
                cnt_str = p + 1;
            }
            long long amt = strtoll(cnt_str, NULL, 10);
            size_t found = (size_t)-1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != (size_t)-1) {
                inv[found].count += amt;
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    InvItem *nb = realloc(inv, cap * sizeof(InvItem));
                    if (!nb) {
                        free(line);
                        break;
                    }
                    inv = nb;
                }
                inv[count].item = safe_dup(item);
                inv[count].count = amt;
                count++;
            }
        } else if (strncmp(line, "TAKE ", 5) == 0) {
            char *p = line + 5;
            char *item = p;
            while (*p && *p != ' ') p++;
            char *cnt_str = "0";
            if (*p == ' ') {
                *p = '\0';
                cnt_str = p + 1;
            }
            long long amt = strtoll(cnt_str, NULL, 10);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    inv[i].count -= amt;
                    if (inv[i].count < 0) {
                        inv[i].count = 0;
                    }
                    break;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *item = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    free(inv[i].item);
                    for (size_t j = i; j < count - 1; j++) {
                        inv[j] = inv[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *item = line + 6;
            long long current = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    current = inv[i].count;
                    break;
                }
            }
            printf("%lld\n", current);
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %lld\n", inv[i].item, inv[i].count);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(inv[i].item);
    }
    free(inv);
    return 0;
}
