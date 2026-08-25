// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *note; } Item;
Item *items = 0; int n = 0, cap = 0;

int find(char *name) {
    for (int i = 0; i < n; i++) if (!strcmp(items[i].name, name)) return i;
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], rest[2048]; int cnt;
        a[0] = rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "BATCH")) sscanf(line, "%*s %d", &cnt);
        else if (!strcmp(cmd, "DELETE") || !strcmp(cmd, "FIND")) sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "BATCH")) {
            Item *batch = malloc(cnt * sizeof(Item));
            int valid = 1;
            for (int i = 0; i < cnt; i++) {
                if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
                line[strcspn(line, "\n")] = 0;
                char nm[256], nt[2048]; nm[0] = nt[0] = 0;
                if (sscanf(line, "%s %[^\n]", nm, nt) < 2) { valid = 0; break; }
                batch[i].name = strdup(nm); batch[i].note = strdup(nt);
            }
            if (valid) {
                for (int i = 0; i < cnt; i++) {
                    if (n == cap) { cap = cap ? cap * 2 : 4; items = realloc(items, cap * sizeof(Item)); }
                    items[n++] = batch[i];
                }
            } else {
                for (int i = 0; i < cnt; i++) { free(batch[i].name); free(batch[i].note); }
            }
            free(batch);
        } else if (!strcmp(cmd, "DELETE")) {
            int idx = find(a);
            if (idx >= 0) {
                free(items[idx].name); free(items[idx].note);
                for (int i = idx; i < n - 1; i++) items[i] = items[i + 1]; n--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            int idx = find(a);
            if (idx >= 0) printf("%s %s\n", items[idx].name, items[idx].note);
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < n; i++) printf("%s %s\n", items[i].name, items[i].note);
        }
    }
    return 0;
}