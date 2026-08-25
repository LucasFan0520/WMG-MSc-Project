// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *data; } Rec;
Rec *recs = 0; int n = 0, cap = 0;

int find(char *name) {
    for (int i = 0; i < n; i++) if (!strcmp(recs[i].name, name)) return i;
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], b[2048]; int len;
        a[0] = b[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "ADD")) {
            if (sscanf(line, "%*s %s %d:%[^\n]", a, &len, b) == 3) {
                if ((int)strlen(b) == len) {
                    int idx = find(a);
                    if (idx >= 0) { free(recs[idx].data); recs[idx].data = strdup(b); }
                    else {
                        if (n == cap) { cap = cap ? cap * 2 : 4; recs = realloc(recs, cap * sizeof(Rec)); }
                        recs[n].name = strdup(a); recs[n].data = strdup(b); n++;
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            sscanf(line, "%*s %s", a);
            int idx = find(a);
            if (idx >= 0) {
                free(recs[idx].name); free(recs[idx].data);
                for (int i = idx; i < n - 1; i++) recs[i] = recs[i + 1]; n--;
            }
        } else if (!strcmp(cmd, "SHOW")) {
            sscanf(line, "%*s %s", a);
            int idx = find(a);
            if (idx >= 0) {
                for (char *c = recs[idx].data; *c; c++) putchar(*c == ' ' ? '_' : *c);
                putchar('\n');
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < n; i++) puts(recs[i].name);
        }
    }
    return 0;
}