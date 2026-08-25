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
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *item = p;
                char *cnt_str = space + 1;
                char *endptr;
                long long val = strtoll(cnt_str, &endptr, 10);
                if (*endptr == '\0' && val >= 0) {
                    int found = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(inv[i].item, item) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found != -1) {
                        inv[found].count += val;
                    } else {
                        InvItem *tmp = realloc(inv, sizeof(InvItem) * (count + 1));
                        if (tmp) {
                            inv = tmp;
                            inv[count].item = strdup(item);
                            inv[count].count = val;
                            if (inv[count].item) {
                                count++;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "TAKE ", 5) == 0) {
            char *p = line + 5;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *item = p;
                char *cnt_str = space + 1;
                char *endptr;
                long long val = strtoll(cnt_str, &endptr, 10);
                if (*endptr == '\0' && val >= 0) {
                    for (int i = 0; i < count; i++) {
                        if (strcmp(inv[i].item, item) == 0) {
                            inv[i].count -= val;
                            if (inv[i].count < 0) inv[i].count = 0;
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *item = line + 7;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(inv[found].item);
                for (int i = found; i < count - 1; i++) {
                    inv[i] = inv[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *item = line + 6;
            long long cnt = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    cnt = inv[i].count;
                    break;
                }
            }
            printf("%lld\n", cnt);
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %lld\n", inv[i].item, inv[i].count);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(inv[i].item);
    }
    free(inv);
    free(line);
    return 0;
}
