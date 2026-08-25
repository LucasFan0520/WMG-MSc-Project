// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEM 100

typedef struct {
    char name[MAX_ITEM];
    int count;
} Item;

Item items[1000];
int item_count = 0;
int insertion_order[1000];
int order_count = 0;

int find_item(const char *name) {
    for (int i = 0; i < item_count; i++) {
        if (strcmp(items[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20], name[MAX_ITEM];
    int count;
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %d", name, &count);
            int idx = find_item(name);
            if (idx == -1) {
                strcpy(items[item_count].name, name);
                items[item_count].count = count;
                insertion_order[order_count++] = item_count;
                item_count++;
            } else {
                items[idx].count += count;
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            scanf("%s %d", name, &count);
            int idx = find_item(name);
            if (idx != -1) {
                items[idx].count -= count;
                if (items[idx].count < 0) items[idx].count = 0;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s", name);
            int idx = find_item(name);
            if (idx != -1) {
                for (int i = idx; i < item_count - 1; i++) {
                    items[i] = items[i + 1];
                }
                item_count--;
                
                for (int i = 0; i < order_count; i++) {
                    if (insertion_order[i] == idx) {
                        for (int j = i; j < order_count - 1; j++) {
                            insertion_order[j] = insertion_order[j + 1];
                        }
                        order_count--;
                        break;
                    } else if (insertion_order[i] > idx) {
                        insertion_order[i]--;
                    }
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            scanf("%s", name);
            int idx = find_item(name);
            if (idx != -1) {
                printf("%d\n", items[idx].count);
            } else {
                printf("0\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < order_count; i++) {
                int idx = insertion_order[i];
                printf("%s %d\n", items[idx].name, items[idx].count);
            }
        }
    }
    
    return 0;
}