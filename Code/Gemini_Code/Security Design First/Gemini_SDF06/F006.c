// F006.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    long long count;
} InvItem;

int main(void) {
    InvItem *inv = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "TAKE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *item = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *cnt_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*item == '\0' || *cnt_str == '\0') continue;
            long long val = strtoll(cnt_str, NULL, 10);

            int found_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    found_idx = (int)i;
                    break;
                }
            }

            if (strcmp(cmd, "ADD") == 0) {
                if (found_idx != -1) {
                    inv[found_idx].count += val;
                } else {
                    if (count >= capacity) {
                        size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                        InvItem *new_inv = realloc(inv, new_cap * sizeof(InvItem));
                        if (!new_inv) continue;
                        inv = new_inv;
                        capacity = new_cap;
                    }
                    char *it = strdup(item);
                    if (it) {
                        inv[count].item = it;
                        inv[count].count = val;
                        count++;
                    }
                }
            } else {
                if (found_idx != -1) {
                    inv[found_idx].count -= val;
                    if (inv[found_idx].count < 0) {
                        inv[found_idx].count = 0;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *item = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*item == '\0') continue;

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
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *item = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*item == '\0') continue;

            long long cnt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    cnt = inv[i].count;
                    break;
                }
            }
            printf("%lld\n", cnt);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %lld\n", inv[i].item, inv[i].count);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(inv[i].item);
    }
    free(inv);
    free(line);
    return 0;
}
