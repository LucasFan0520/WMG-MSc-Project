// F006.c
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
    size_t cap = 0;
    ssize_t read;
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char name[256];
            long long amt;
            if (sscanf(line + 4, "%255s %lld", name, &amt) == 2) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(inv[i].item, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    inv[found].count += amt;
                } else {
                    InvItem *tmp = realloc(inv, (count + 1) * sizeof(InvItem));
                    if (tmp) {
                        inv = tmp;
                        inv[count].item = strdup(name);
                        inv[count].count = amt;
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "TAKE ", 5) == 0) {
            char name[256];
            long long amt;
            if (sscanf(line + 5, "%255s %lld", name, &amt) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(inv[i].item, name) == 0) {
                        inv[i].count -= amt;
                        if (inv[i].count < 0) {
                            inv[i].count = 0;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') {
                name++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, name) == 0) {
                    free(inv[i].item);
                    for (int j = i; j < count - 1; j++) {
                        inv[j] = inv[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *name = line + 6;
            while (*name == ' ') {
                name++;
            }
            long long ans = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(inv[i].item, name) == 0) {
                    ans = inv[i].count;
                    break;
                }
            }
            printf("%lld\n", ans);
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %lld\n", inv[i].item, inv[i].count);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(inv[i].item);
    }
    free(inv);
    return 0;
}
