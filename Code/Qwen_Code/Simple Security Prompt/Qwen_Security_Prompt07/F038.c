// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *enc; } RLE;
RLE *rles = 0; int n = 0, cap = 0;

int find(char *name) {
    for (int i = 0; i < n; i++) if (!strcmp(rles[i].name, name)) return i;
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], b[2048];
        a[0] = b[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "STORE")) sscanf(line, "%*s %s %s", a, b);
        else sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "STORE")) {
            int idx = find(a);
            if (idx >= 0) { free(rles[idx].enc); rles[idx].enc = strdup(b); }
            else {
                if (n == cap) { cap = cap ? cap * 2 : 4; rles = realloc(rles, cap * sizeof(RLE)); }
                rles[n].name = strdup(a); rles[n].enc = strdup(b); n++;
            }
        } else if (!strcmp(cmd, "DECODE")) {
            int idx = find(a);
            if (idx >= 0) {
                char *p = rles[idx].enc;
                int valid = 1;
                while (*p) {
                    int cnt = 0;
                    while (*p >= '0' && *p <= '9') { cnt = cnt * 10 + (*p - '0'); p++; if (cnt > 1000000) { valid = 0; break; } }
                    if (!valid || !*p) { valid = 0; break; }
                    p++;
                }
                if (valid) {
                    p = rles[idx].enc;
                    while (*p) {
                        int cnt = 0;
                        while (*p >= '0' && *p <= '9') { cnt = cnt * 10 + (*p - '0'); p++; }
                        char c = *p++;
                        if (c == ' ') c = '_';
                        for (int i = 0; i < cnt; i++) putchar(c);
                    }
                    putchar('\n');
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            int idx = find(a);
            if (idx >= 0) {
                free(rles[idx].name); free(rles[idx].enc);
                for (int i = idx; i < n - 1; i++) rles[i] = rles[i + 1]; n--;
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < n; i++) puts(rles[i].name);
        }
    }
    return 0;
}