// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *desc;
} Ev;

typedef struct {
    char *group;
    Ev *evs;
    int n;
} Grp;

Grp *grps = NULL;
int ng = 0;

int find_grp(const char *g) {
    for (int i = 0; i < ng; i++)
        if (strcmp(grps[i].group, g) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], arg1[1000], arg2[1000], arg3[1000];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "EVENT") == 0) {
            sscanf(line, "%*s %s", arg1);
            char *p = line + 6 + strlen(arg1);
            if (*p == ' ') p++;
            int i = find_grp(arg1);
            if (i == -1) {
                grps = realloc(grps, sizeof(Grp) * (ng + 1));
                grps[ng].group = strdup(arg1);
                grps[ng].evs = NULL;
                grps[ng].n = 0;
                i = ng++;
            }
            grps[i].evs = realloc(grps[i].evs, sizeof(Ev) * (grps[i].n + 1));
            grps[i].evs[grps[i].n++].desc = strdup(p);
        } else if (strcmp(cmd, "MERGE") == 0) {
            sscanf(line, "%*s %s %s %s", arg1, arg2, arg3);
            int i1 = find_grp(arg1), i2 = find_grp(arg2);
            int i3 = find_grp(arg3);
            if (i3 == -1) {
                grps = realloc(grps, sizeof(Grp) * (ng + 1));
                grps[ng].group = strdup(arg3);
                grps[ng].evs = NULL;
                grps[ng].n = 0;
                i3 = ng++;
            }
            if (i1 != -1) {
                for (int j = 0; j < grps[i1].n; j++) {
                    grps[i3].evs = realloc(grps[i3].evs, sizeof(Ev) * (grps[i3].n + 1));
                    grps[i3].evs[grps[i3].n++].desc = strdup(grps[i1].evs[j].desc);
                }
            }
            if (i2 != -1) {
                for (int j = 0; j < grps[i2].n; j++) {
                    grps[i3].evs = realloc(grps[i3].evs, sizeof(Ev) * (grps[i3].n + 1));
                    grps[i3].evs[grps[i3].n++].desc = strdup(grps[i2].evs[j].desc);
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_grp(arg1);
            if (i != -1) {
                free(grps[i].group);
                for (int j = 0; j < grps[i].n; j++) free(grps[i].evs[j].desc);
                free(grps[i].evs);
                for (int j = i; j < ng - 1; j++) grps[j] = grps[j + 1];
                ng--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_grp(arg1);
            printf("%d\n", i != -1 ? grps[i].n : 0);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < ng; i++) {
                printf("%s:", grps[i].group);
                for (int j = 0; j < grps[i].n; j++) printf(" %s", grps[i].evs[j].desc);
                printf("\n");
            }
        }
    }
    for (int i = 0; i < ng; i++) {
        free(grps[i].group);
        for (int j = 0; j < grps[i].n; j++) free(grps[i].evs[j].desc);
        free(grps[i].evs);
    }
    free(grps);
    return 0;
}