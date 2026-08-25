// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} InvItem;

InvItem *inv = NULL;
int count_items = 0;
int capacity = 0;

int find_item(const char *item) {
    for (int i = 0; i < count_items; i++) {
        if (strcmp(inv[i].item, item) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p1 = line + 4;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                int amt = atoi(p2 + 1);
                int idx = find_item(p1);
                if (idx != -1) {
                    inv[idx].count += amt;
                } else {
                    if (count_items >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        inv = realloc(inv, capacity * sizeof(InvItem));
                    }
                    inv[count_items].item = strdup(p1);
                    inv[count_items].count = amt;
                    count_items++;
                }
            }
        } else if (strncmp(line, "TAKE ", 5) == 0) {
            char *p1 = line + 5;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                int amt = atoi(p2 + 1);
                int idx = find_item(p1);
                if (idx != -1) {
                    inv[idx].count -= amt;
                    if (inv[idx].count < 0) {
                        inv[idx].count = 0;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *item = line + 7;
            int idx = find_item(item);
            if (idx != -1) {
                free(inv[idx].item);
                for (int i = idx; i < count_items - 1; i++) {
                    inv[i] = inv[i + 1];
                }
                count_items--;
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *item = line + 6;
            int idx = find_item(item);
            if (idx != -1) {
                printf("%d\n", inv[idx].count);
            } else {
                printf("0\n");
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count_items; i++) {
                printf("%s %d\n", inv[i].item, inv[i].count);
            }
        }
    }
    for (int i = 0; i < count_items; i++) {
        free(inv[i].item);
    }
    free(inv);
    return 0;
}
