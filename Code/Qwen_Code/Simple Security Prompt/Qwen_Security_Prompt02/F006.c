// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} Inventory;

Inventory *inv = NULL;
int total = 0;

int find_item(const char *item) {
    for (int i = 0; i < total; i++) {
        if (strcmp(inv[i].item, item) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char cmd[20];
    char item[1000];
    int c;

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %d", item, &c);
            int idx = find_item(item);
            if (idx != -1) {
                inv[idx].count += c;
            } else {
                inv = realloc(inv, sizeof(Inventory) * (total + 1));
                inv[total].item = strdup(item);
                inv[total].count = c;
                total++;
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            scanf("%s %d", item, &c);
            int idx = find_item(item);
            if (idx != -1) {
                inv[idx].count -= c;
                if (inv[idx].count < 0) {
                    inv[idx].count = 0;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s", item);
            int idx = find_item(item);
            if (idx != -1) {
                free(inv[idx].item);
                for (int i = idx; i < total - 1; i++) {
                    inv[i] = inv[i + 1];
                }
                total--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            scanf("%s", item);
            int idx = find_item(item);
            if (idx != -1) {
                printf("%d\n", inv[idx].count);
            } else {
                printf("0\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < total; i++) {
                printf("%s %d\n", inv[i].item, inv[i].count);
            }
        }
    }

    for (int i = 0; i < total; i++) {
        free(inv[i].item);
    }
    free(inv);

    return 0;
}