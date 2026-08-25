// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *item; int count; } Inv;
Inv inv[1000]; int ni = 0;

int find(char *item) {
    for (int i = 0; i < ni; i++) if (!strcmp(inv[i].item, item)) return i;
    return -1;
}

int main() {
    char cmd[20], item[100]; int count;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s %d", item, &count);
            int idx = find(item);
            if (idx != -1) inv[idx].count += count;
            else { inv[ni].item = strdup(item); inv[ni++].count = count; }
        } else if (!strcmp(cmd, "TAKE")) {
            scanf("%s %d", item, &count);
            int idx = find(item);
            if (idx != -1) { inv[idx].count -= count; if (inv[idx].count < 0) inv[idx].count = 0; }
        } else if (!strcmp(cmd, "REMOVE")) {
            scanf("%s", item); int idx = find(item);
            if (idx != -1) {
                free(inv[idx].item);
                for (int i = idx; i < ni - 1; i++) inv[i] = inv[i + 1];
                ni--;
            }
        } else if (!strcmp(cmd, "COUNT")) {
            scanf("%s", item); int idx = find(item);
            printf("%d\n", idx != -1 ? inv[idx].count : 0);
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < ni; i++) printf("%s %d\n", inv[i].item, inv[i].count);
        }
    }
    for (int i = 0; i < ni; i++) free(inv[i].item);
    return 0;
}