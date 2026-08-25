// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *item; int count; } Item;
Item *items = 0; int n = 0, cap = 0;

int find(char *item) {
    for (int i=0; i<n; i++) if (!strcmp(items[i].item, item)) return i;
    return -1;
}

int main() {
    char line[2048];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], item[256]; int count;
        sscanf(line, "%s %s %d", cmd, item, &count);
        if (!strcmp(cmd, "ADD")) {
            int i = find(item);
            if (i >= 0) items[i].count += count;
            else {
                if (n == cap) { cap = cap ? cap*2 : 4; items = realloc(items, cap*sizeof(Item)); }
                items[n].item = strdup(item); items[n].count = count; n++;
            }
        } else if (!strcmp(cmd, "TAKE")) {
            int i = find(item);
            if (i >= 0) { items[i].count -= count; if (items[i].count < 0) items[i].count = 0; }
        } else if (!strcmp(cmd, "REMOVE")) {
            int i = find(item);
            if (i >= 0) {
                free(items[i].item);
                for (int j=i; j<n-1; j++) items[j] = items[j+1]; n--;
            }
        } else if (!strcmp(cmd, "COUNT")) {
            int i = find(item); printf("%d\n", i >= 0 ? items[i].count : 0);
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i=0; i<n; i++) printf("%s %d\n", items[i].item, items[i].count);
        }
    }
    return 0;
}