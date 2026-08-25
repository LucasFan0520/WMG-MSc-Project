// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *item; int count; } Inv;
Inv *inv = NULL; int ic = 0;

int find_item(const char *n) {
    for (int i = 0; i < ic; i++) if (strcmp(inv[i].item, n) == 0) return i;
    return -1;
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char c1[20], c2[256]; int cnt;
        c2[0] = 0;
        sscanf(line, "%19s %255s %d", c1, c2, &cnt);
        
        if (strcmp(c1, "ADD") == 0) {
            int i = find_item(c2);
            if (i >= 0) inv[i].count += cnt;
            else { inv = realloc(inv, (ic+1)*sizeof(Inv)); inv[ic].item = strdup(c2); inv[ic].count = cnt; ic++; }
        } else if (strcmp(c1, "TAKE") == 0) {
            int i = find_item(c2);
            if (i >= 0) { inv[i].count -= cnt; if (inv[i].count < 0) inv[i].count = 0; }
        } else if (strcmp(c1, "REMOVE") == 0) {
            int i = find_item(c2);
            if (i >= 0) { free(inv[i].item); for(int j=i; j<ic-1; j++) inv[j]=inv[j+1]; ic--; }
        } else if (strcmp(c1, "COUNT") == 0) {
            int i = find_item(c2);
            printf("%d\n", i >= 0 ? inv[i].count : 0);
        } else if (strcmp(c1, "REPORT") == 0) {
            for (int i = 0; i < ic; i++) printf("%s %d\n", inv[i].item, inv[i].count);
        }
    }
    for (int i = 0; i < ic; i++) free(inv[i].item);
    free(inv);
    return 0;
}