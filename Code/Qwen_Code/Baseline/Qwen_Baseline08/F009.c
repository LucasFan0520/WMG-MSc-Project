// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_GROUPS 1000
#define MAX_EVENTS 10000
#define MAX_LEN 1000

typedef struct {
    char name[100];
    char events[MAX_EVENTS][MAX_LEN];
    int ecount;
} Group;

Group groups[MAX_GROUPS];
int gcount = 0;

int find_group(const char *name) {
    for (int i = 0; i < gcount; i++) if (strcmp(groups[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], g1[100], g2[100], g3[100], desc[MAX_LEN];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "EVENT") == 0) {
            scanf("%s", g1);
            fgets(desc, MAX_LEN, stdin);
            desc[strcspn(desc, "\n")] = 0;
            if (desc[0] == ' ') memmove(desc, desc + 1, strlen(desc));
            int idx = find_group(g1);
            if (idx == -1 && gcount < MAX_GROUPS) {
                strcpy(groups[gcount].name, g1);
                groups[gcount].ecount = 0;
                idx = gcount++;
            }
            if (idx != -1 && groups[idx].ecount < MAX_EVENTS) {
                strcpy(groups[idx].events[groups[idx].ecount++], desc);
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            scanf("%s %s %s", g1, g2, g3);
            int i1 = find_group(g1), i2 = find_group(g2);
            int i3 = find_group(g3);
            if (i3 == -1 && gcount < MAX_GROUPS) {
                strcpy(groups[gcount].name, g3);
                groups[gcount].ecount = 0;
                i3 = gcount++;
            }
            if (i1 != -1 && i2 != -1 && i3 != -1) {
                for (int i = 0; i < groups[i1].ecount && groups[i3].ecount < MAX_EVENTS; i++)
                    strcpy(groups[i3].events[groups[i3].ecount++], groups[i1].events[i]);
                for (int i = 0; i < groups[i2].ecount && groups[i3].ecount < MAX_EVENTS; i++)
                    strcpy(groups[i3].events[groups[i3].ecount++], groups[i2].events[i]);
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            scanf("%s", g1);
            int idx = find_group(g1);
            if (idx != -1) {
                for (int i = idx; i < gcount - 1; i++) groups[i] = groups[i + 1];
                gcount--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            scanf("%s", g1);
            int idx = find_group(g1);
            printf("%d\n", idx != -1 ? groups[idx].ecount : 0);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < gcount; i++) {
                printf("%s:", groups[i].name);
                for (int j = 0; j < groups[i].ecount; j++) printf(" %s", groups[i].events[j]);
                printf("\n");
            }
        }
    }
    return 0;
}