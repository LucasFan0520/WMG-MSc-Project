// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} Item;

Item *items = NULL;
int item_count = 0;

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char item[100];
            int c;
            scanf("%s %d", item, &c);
            int idx = -1;
            for (int i = 0; i < item_count; i++) {
                if (strcmp(items[i].item, item) == 0) { idx = i; break; }
            }
            if (idx != -1) {
                items[idx].count += c;
            } else {
                items = realloc(items, sizeof(Item) * (item_count + 1));
                items[item_count].item = strdup(item);
                items[item_count].count = c;
                item_count++;
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char item[100];
            int c;
            scanf("%s %d", item, &c);
            for (int i = 0; i < item_count; i++) {
                if (strcmp(items[i].item, item) == 0) {
                    items[i].count -= c;
                    if (items[i].count < 0) items[i].count = 0;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char item[100];
            scanf("%s", item);
            for (int i = 0; i < item_count; i++) {
                if (strcmp(items[i].item, item) == 0) {
                    free(items[i].item);
                    for (int j = i; j < item_count - 1; j++) items[j] = items[j+1];
                    item_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char item[100];
            scanf("%s", item);
            int found = 0;
            for (int i = 0; i < item_count; i++) {
                if (strcmp(items[i].item, item) == 0) {
                    printf("%d\n", items[i].count);
                    found = 1;
                    break;
                }
            }
            if (!found) printf("0\n");
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < item_count; i++) {
                printf("%s %d\n", items[i].item, items[i].count);
            }
        }
    }
    for (int i = 0; i < item_count; i++) free(items[i].item);
    free(items);
    return 0;
}