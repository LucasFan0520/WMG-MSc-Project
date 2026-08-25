// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} Inv;

Inv inv[1000];
int ic = 0;
char *ins_order[1000];
int ioc = 0;

int find_item(char *item) {
    for (int i = 0; i < ic; i++) if (strcmp(inv[i].item, item) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char item[1000];
            int c;
            sscanf(rest, "%s %d", item, &c);
            int idx = find_item(item);
            if (idx == -1) {
                inv[ic].item = strdup(item);
                inv[ic].count = c;
                ins_order[ioc++] = strdup(item);
                ic++;
            } else {
                inv[idx].count += c;
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char item[1000];
            int c;
            sscanf(rest, "%s %d", item, &c);
            int idx = find_item(item);
            if (idx != -1) {
                inv[idx].count -= c;
                if (inv[idx].count < 0) inv[idx].count = 0;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char item[1000];
            sscanf(rest, "%s", item);
            int idx = find_item(item);
            if (idx != -1) {
                free(inv[idx].item);
                for (int i = idx; i < ic - 1; i++) inv[i] = inv[i+1];
                ic--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char item[1000];
            sscanf(rest, "%s", item);
            int idx = find_item(item);
            if (idx != -1) printf("%d\n", inv[idx].count);
            else printf("0\n");
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < ioc; i++) {
                int idx = find_item(ins_order[i]);
                if (idx != -1) printf("%s %d\n", inv[idx].item, inv[idx].count);
            }
        }
    }
    for (int i = 0; i < ic; i++) free(inv[i].item);
    for (int i = 0; i < ioc; i++) free(ins_order[i]);
    return 0;
}