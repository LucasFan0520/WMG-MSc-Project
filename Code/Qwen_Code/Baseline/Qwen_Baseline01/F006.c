// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} Inv;

Inv *inv = NULL;
int count = 0;

int find_item(const char *item) {
    for (int i = 0; i < count; i++)
        if (strcmp(inv[i].item, item) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], arg1[1000];
        int c;
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s %d", arg1, &c);
            int i = find_item(arg1);
            if (i != -1) inv[i].count += c;
            else {
                inv = realloc(inv, sizeof(Inv) * (count + 1));
                inv[count].item = strdup(arg1);
                inv[count].count = c;
                count++;
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            sscanf(line, "%*s %s %d", arg1, &c);
            int i = find_item(arg1);
            if (i != -1) {
                inv[i].count -= c;
                if (inv[i].count < 0) inv[i].count = 0;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_item(arg1);
            if (i != -1) {
                free(inv[i].item);
                for (int j = i; j < count - 1; j++) inv[j] = inv[j + 1];
                count--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_item(arg1);
            printf("%d\n", i != -1 ? inv[i].count : 0);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) printf("%s %d\n", inv[i].item, inv[i].count);
        }
    }
    for (int i = 0; i < count; i++) free(inv[i].item);
    free(inv);
    return 0;
}