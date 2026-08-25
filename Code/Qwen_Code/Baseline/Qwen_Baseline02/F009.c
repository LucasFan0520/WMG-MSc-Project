// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *desc; } Ev;
typedef struct { char *name; Ev *evs; int ne; } Grp;
Grp g[1000]; int ng = 0;

int find(char *name) {
    for (int i = 0; i < ng; i++) if (!strcmp(g[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], n1[100], n2[100], nn[100], desc[10000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "EVENT")) {
            scanf("%s", n1); fgets(desc, sizeof(desc), stdin);
            desc[strcspn(desc, "\n")] = 0;
            if (desc[0] == ' ') memmove(desc, desc + 1, strlen(desc));
            int idx = find(n1);
            if (idx == -1) {
                g[ng].name = strdup(n1); g[ng].evs = malloc(sizeof(Ev));
                g[ng].evs[0].desc = strdup(desc); g[ng++].ne = 1;
            } else {
                g[idx].evs = realloc(g[idx].evs, sizeof(Ev) * (g[idx].ne + 1));
                g[idx].evs[g[idx].ne++].desc = strdup(desc);
            }
        } else if (!strcmp(cmd, "MERGE")) {
            scanf("%s %s %s", n1, n2, nn);
            int i1 = find(n1), i2 = find(n2);
            if (i1 != -1 && i2 != -1) {
                int ne = g[i1].ne + g[i2].ne;
                g[ng].name = strdup(nn); g[ng].evs = malloc(sizeof(Ev) * ne); g[ng].ne = 0;
                for (int i = 0; i < g[i1].ne; i++) g[ng].evs[g[ng].ne++].desc = strdup(g[i1].evs[i].desc);
                for (int i = 0; i < g[i2].ne; i++) g[ng].evs[g[ng].ne++].desc = strdup(g[i2].evs[i].desc);
                ng++;
            }
        } else if (!strcmp(cmd, "DELETEGROUP")) {
            scanf("%s", n1); int idx = find(n1);
            if (idx != -1) {
                free(g[idx].name);
                for (int i = 0; i < g[idx].ne; i++) free(g[idx].evs[i].desc);
                free(g[idx].evs);
                for (int i = idx; i < ng - 1; i++) g[i] = g[i + 1];
                ng--;
            }
        } else if (!strcmp(cmd, "COUNT")) {
            scanf("%s", n1); int idx = find(n1);
            printf("%d\n", idx != -1 ? g[idx].ne : 0);
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < ng; i++) {
                printf("%s:", g[i].name);
                for (int j = 0; j < g[i].ne; j++) printf(" %s", g[i].evs[j].desc);
                printf("\n");
            }
        }
    }
    for (int i = 0; i < ng; i++) {
        free(g[i].name);
        for (int j = 0; j < g[i].ne; j++) free(g[i].evs[j].desc);
        free(g[i].evs);
    }
    return 0;
}