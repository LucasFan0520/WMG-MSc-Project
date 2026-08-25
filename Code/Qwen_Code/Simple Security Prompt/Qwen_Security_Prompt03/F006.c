// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *item; int count; } Inv;
Inv *inv = NULL;
int n = 0;

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], item[256];
        int c;
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s %d", item, &c);
            int idx = -1;
            for (int i = 0; i < n; i++) if (strcmp(inv[i].item, item) == 0) { idx = i; break; }
            if (idx != -1) inv[idx].count += c;
            else { inv = realloc(inv, (n + 1) * sizeof(Inv)); inv[n].item = strdup(item); inv[n].count = c; n++; }
        } else if (strcmp(cmd, "TAKE") == 0) {
            sscanf(line, "%*s %s %d", item, &c);
            for (int i = 0; i < n; i++) if (strcmp(inv[i].item, item) == 0) { inv[i].count -= c; if (inv[i].count < 0) inv[i].count = 0; }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            sscanf(line, "%*s %s", item);
            for (int i = 0; i < n; i++) {
                if (strcmp(inv[i].item, item) == 0) {
                    free(inv[i].item);
                    for (int j = i; j < n - 1; j++) inv[j] = inv[j + 1];
                    n--; break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            sscanf(line, "%*s %s", item);
            int found = 0;
            for (int i = 0; i < n; i++) if (strcmp(inv[i].item, item) == 0) { printf("%d\n", inv[i].count); found = 1; break; }
            if (!found) printf("0\n");
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < n; i++) printf("%s %d\n", inv[i].item, inv[i].count);
        }
    }
    for (int i = 0; i < n; i++) free(inv[i].item);
    free(inv);
    return 0;
}