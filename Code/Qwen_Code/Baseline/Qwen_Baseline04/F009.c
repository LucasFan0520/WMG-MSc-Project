// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char **descs;
    int count;
} Group;

typedef struct {
    char *name;
    Group g;
} GroupEntry;

GroupEntry *groups = NULL;
int group_count = 0;

int find_group(const char *name) {
    for (int i = 0; i < group_count; i++) {
        if (strcmp(groups[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "EVENT") == 0) {
            char group[100], desc[1000];
            scanf("%s", group);
            scanf(" %[^\n]", desc);
            int idx = find_group(group);
            if (idx == -1) {
                groups = realloc(groups, sizeof(GroupEntry) * (group_count + 1));
                groups[group_count].name = strdup(group);
                groups[group_count].g.descs = NULL;
                groups[group_count].g.count = 0;
                idx = group_count++;
            }
            groups[idx].g.descs = realloc(groups[idx].g.descs, sizeof(char*) * (groups[idx].g.count + 1));
            groups[idx].g.descs[groups[idx].g.count++] = strdup(desc);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char gA[100], gB[100], newG[100];
            scanf("%s %s %s", gA, gB, newG);
            int iA = find_group(gA);
            int iB = find_group(gB);
            int iN = find_group(newG);
            if (iN == -1) {
                groups = realloc(groups, sizeof(GroupEntry) * (group_count + 1));
                groups[group_count].name = strdup(newG);
                groups[group_count].g.descs = NULL;
                groups[group_count].g.count = 0;
                iN = group_count++;
            }
            if (iA != -1) {
                for (int i = 0; i < groups[iA].g.count; i++) {
                    groups[iN].g.descs = realloc(groups[iN].g.descs, sizeof(char*) * (groups[iN].g.count + 1));
                    groups[iN].g.descs[groups[iN].g.count++] = strdup(groups[iA].g.descs[i]);
                }
            }
            if (iB != -1) {
                for (int i = 0; i < groups[iB].g.count; i++) {
                    groups[iN].g.descs = realloc(groups[iN].g.descs, sizeof(char*) * (groups[iN].g.count + 1));
                    groups[iN].g.descs[groups[iN].g.count++] = strdup(groups[iB].g.descs[i]);
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char group[100];
            scanf("%s", group);
            int idx = find_group(group);
            if (idx != -1) {
                free(groups[idx].name);
                for (int i = 0; i < groups[idx].g.count; i++) free(groups[idx].g.descs[i]);
                free(groups[idx].g.descs);
                for (int j = idx; j < group_count - 1; j++) groups[j] = groups[j+1];
                group_count--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char group[100];
            scanf("%s", group);
            int idx = find_group(group);
            if (idx != -1) printf("%d\n", groups[idx].g.count);
            else printf("0\n");
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < group_count; i++) {
                printf("%s", groups[i].name);
                for (int j = 0; j < groups[i].g.count; j++) {
                    printf(" %s", groups[i].g.descs[j]);
                }
                printf("\n");
            }
        }
    }
    for (int i = 0; i < group_count; i++) {
        free(groups[i].name);
        for (int j = 0; j < groups[i].g.count; j++) free(groups[i].g.descs[j]);
        free(groups[i].g.descs);
    }
    free(groups);
    return 0;
}