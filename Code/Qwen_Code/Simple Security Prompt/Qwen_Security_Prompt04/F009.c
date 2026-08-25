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
    int ec;
} Grp;

Grp grps[1000];
int gc = 0;

int find_grp(char *group) {
    for (int i = 0; i < gc; i++) if (strcmp(grps[i].group, group) == 0) return i;
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
        
        if (strcmp(cmd, "EVENT") == 0) {
            char group[1000];
            sscanf(rest, "%s", group);
            char *desc = rest + strlen(group);
            while (*desc == ' ') desc++;
            int idx = find_grp(group);
            if (idx == -1) {
                grps[gc].group = strdup(group);
                grps[gc].evs = NULL;
                grps[gc].ec = 0;
                idx = gc++;
            }
            grps[idx].evs = realloc(grps[idx].evs, sizeof(Ev) * (grps[idx].ec + 1));
            grps[idx].evs[grps[idx].ec].desc = strdup(desc);
            grps[idx].ec++;
        } else if (strcmp(cmd, "MERGE") == 0) {
            char gA[1000], gB[1000], nG[1000];
            sscanf(rest, "%s %s %s", gA, gB, nG);
            int iA = find_grp(gA), iB = find_grp(gB);
            int iN = find_grp(nG);
            if (iN == -1) {
                grps[gc].group = strdup(nG);
                grps[gc].evs = NULL;
                grps[gc].ec = 0;
                iN = gc++;
            }
            if (iA != -1) {
                for (int i = 0; i < grps[iA].ec; i++) {
                    grps[iN].evs = realloc(grps[iN].evs, sizeof(Ev) * (grps[iN].ec + 1));
                    grps[iN].evs[grps[iN].ec].desc = strdup(grps[iA].evs[i].desc);
                    grps[iN].ec++;
                }
            }
            if (iB != -1) {
                for (int i = 0; i < grps[iB].ec; i++) {
                    grps[iN].evs = realloc(grps[iN].evs, sizeof(Ev) * (grps[iN].ec + 1));
                    grps[iN].evs[grps[iN].ec].desc = strdup(grps[iB].evs[i].desc);
                    grps[iN].ec++;
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char group[1000];
            sscanf(rest, "%s", group);
            int idx = find_grp(group);
            if (idx != -1) {
                free(grps[idx].group);
                for (int i = 0; i < grps[idx].ec; i++) free(grps[idx].evs[i].desc);
                free(grps[idx].evs);
                for (int i = idx; i < gc - 1; i++) grps[i] = grps[i+1];
                gc--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char group[1000];
            sscanf(rest, "%s", group);
            int idx = find_grp(group);
            if (idx != -1) printf("%d\n", grps[idx].ec);
            else printf("0\n");
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < gc; i++) {
                printf("%s:", grps[i].group);
                for (int j = 0; j < grps[i].ec; j++) {
                    printf(" %s", grps[i].evs[j].desc);
                }
                printf("\n");
            }
        }
    }
    for (int i = 0; i < gc; i++) {
        free(grps[i].group);
        for (int j = 0; j < grps[i].ec; j++) free(grps[i].evs[j].desc);
        free(grps[i].evs);
    }
    return 0;
}