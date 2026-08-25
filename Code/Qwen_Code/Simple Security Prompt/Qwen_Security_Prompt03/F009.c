// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *desc; } Ev;
typedef struct { char *name; Ev *evs; int cnt; } Grp;
Grp *g = NULL;
int ng = 0;

int find_grp(const char *name) {
    for (int i = 0; i < ng; i++) if (strcmp(g[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256], b[256], c[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "EVENT") == 0) {
            sscanf(line, "%*s %s", a);
            char *desc = strchr(line, ' '); desc++; desc = strchr(desc, ' ');
            int idx = find_grp(a);
            if (idx == -1) { g = realloc(g, (ng + 1) * sizeof(Grp)); g[ng].name = strdup(a); g[ng].evs = NULL; g[ng].cnt = 0; idx = ng++; }
            g[idx].evs = realloc(g[idx].evs, (g[idx].cnt + 1) * sizeof(Ev));
            g[idx].evs[g[idx].cnt++].desc = strdup(desc ? desc + 1 : "");
        } else if (strcmp(cmd, "MERGE") == 0) {
            sscanf(line, "%*s %s %s %s", a, b, c);
            int ia = find_grp(a), ib = find_grp(b);
            int ic = find_grp(c);
            if (ic == -1) { g = realloc(g, (ng + 1) * sizeof(Grp)); g[ng].name = strdup(c); g[ng].evs = NULL; g[ng].cnt = 0; ic = ng++; }
            if (ia != -1) for (int i = 0; i < g[ia].cnt; i++) { g[ic].evs = realloc(g[ic].evs, (g[ic].cnt + 1) * sizeof(Ev)); g[ic].evs[g[ic].cnt++].desc = strdup(g[ia].evs[i].desc); }
            if (ib != -1) for (int i = 0; i < g[ib].cnt; i++) { g[ic].evs = realloc(g[ic].evs, (g[ic].cnt + 1) * sizeof(Ev)); g[ic].evs[g[ic].cnt++].desc = strdup(g[ib].evs[i].desc); }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            sscanf(line, "%*s %s", a);
            int idx = find_grp(a);
            if (idx != -1) {
                for (int i = 0; i < g[idx].cnt; i++) free(g[idx].evs[i].desc);
                free(g[idx].evs); free(g[idx].name);
                for (int i = idx; i < ng - 1; i++) g[i] = g[i + 1];
                ng--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            sscanf(line, "%*s %s", a);
            int idx = find_grp(a);
            printf("%d\n", idx != -1 ? g[idx].cnt : 0);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < ng; i++) {
                printf("%s:", g[i].name);
                for (int j = 0; j < g[i].cnt; j++) printf(" %s", g[i].evs[j].desc);
                printf("\n");
            }
        }
    }
    for (int i = 0; i < ng; i++) {
        for (int j = 0; j < g[i].cnt; j++) free(g[i].evs[j].desc);
        free(g[i].evs); free(g[i].name);
    }
    free(g);
    return 0;
}