// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 1000

typedef struct {
    char name[100];
    int count;
} Item;

Item items[MAX_ITEMS];
int count = 0;
int order[MAX_ITEMS];
int order_count = 0;

int find_item(const char *name) {
    for (int i = 0; i < count; i++) if (strcmp(items[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], name[100];
    int c;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %d", name, &c);
            int idx = find_item(name);
            if (idx == -1 && count < MAX_ITEMS) {
                strcpy(items[count].name, name);
                items[count].count = c;
                order[order_count++] = count;
                count++;
            } else if (idx != -1) {
                items[idx].count += c;
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            scanf("%s %d", name, &c);
            int idx = find_item(name);
            if (idx != -1) {
                items[idx].count -= c;
                if (items[idx].count < 0) items[idx].count = 0;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s", name);
            int idx = find_item(name);
            if (idx != -1) {
                for (int i = idx; i < count - 1; i++) items[i] = items[i + 1];
                count--;
                for (int i = 0; i < order_count; i++) {
                    if (order[i] == idx) {
                        for (int j = i; j < order_count - 1; j++) order[j] = order[j + 1];
                        order_count--;
                        break;
                    } else if (order[i] > idx) {
                        order[i]--;
                    }
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            scanf("%s", name);
            int idx = find_item(name);
            printf("%d\n", idx != -1 ? items[idx].count : 0);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < order_count; i++) {
                int idx = order[i];
                printf("%s %d\n", items[idx].name, items[idx].count);
            }
        }
    }
    return 0;
}