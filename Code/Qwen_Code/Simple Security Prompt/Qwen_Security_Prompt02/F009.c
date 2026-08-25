// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *desc;
} Event;

typedef struct {
    char *group;
    Event *events;
    int count;
    int cap;
} Group;

Group *groups = NULL;
int total = 0;

int find_group(const char *group) {
    for (int i = 0; i < total; i++) {
        if (strcmp(groups[i].group, group) == 0) {
            return i;
        }
    }
    return -1;
}

void add_event(int idx, const char *desc) {
    if (groups[idx].count == groups[idx].cap) {
        groups[idx].cap = groups[idx].cap ? groups[idx].cap * 2 : 4;
        groups[idx].events = realloc(groups[idx].events, sizeof(Event) * groups[idx].cap);
    }
    groups[idx].events[groups[idx].count++].desc = strdup(desc);
}

int main() {
    char cmd[20];
    char g1[1000];
    char g2[1000];
    char g3[1000];
    char desc[10000];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "EVENT") == 0) {
            scanf("%s", g1);
            fgets(desc, sizeof(desc), stdin);
            if (desc[0] == ' ') {
                memmove(desc, desc + 1, strlen(desc));
            }
            desc[strcspn(desc, "\n")] = 0;
            int idx = find_group(g1);
            if (idx == -1) {
                groups = realloc(groups, sizeof(Group) * (total + 1));
                groups[total].group = strdup(g1);
                groups[total].events = NULL;
                groups[total].count = 0;
                groups[total].cap = 0;
                idx = total++;
            }
            add_event(idx, desc);
        } else if (strcmp(cmd, "MERGE") == 0) {
            scanf("%s %s %s", g1, g2, g3);
            int i1 = find_group(g1);
            int i2 = find_group(g2);
            int i3 = find_group(g3);
            if (i3 == -1) {
                groups = realloc(groups, sizeof(Group) * (total + 1));
                groups[total].group = strdup(g3);
                groups[total].events = NULL;
                groups[total].count = 0;
                groups[total].cap = 0;
                i3 = total++;
            }
            if (i1 != -1) {
                for (int i = 0; i < groups[i1].count; i++) {
                    add_event(i3, groups[i1].events[i].desc);
                }
            }
            if (i2 != -1) {
                for (int i = 0; i < groups[i2].count; i++) {
                    add_event(i3, groups[i2].events[i].desc);
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            scanf("%s", g1);
            int idx = find_group(g1);
            if (idx != -1) {
                free(groups[idx].group);
                for (int i = 0; i < groups[idx].count; i++) {
                    free(groups[idx].events[i].desc);
                }
                free(groups[idx].events);
                for (int i = idx; i < total - 1; i++) {
                    groups[i] = groups[i + 1];
                }
                total--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            scanf("%s", g1);
            int idx = find_group(g1);
            if (idx != -1) {
                printf("%d\n", groups[idx].count);
            } else {
                printf("0\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < total; i++) {
                printf("%s:", groups[i].group);
                for (int j = 0; j < groups[i].count; j++) {
                    printf(" %s", groups[i].events[j].desc);
                }
                printf("\n");
            }
        }
    }

    for (int i = 0; i < total; i++) {
        free(groups[i].group);
        for (int j = 0; j < groups[i].count; j++) {
            free(groups[i].events[j].desc);
        }
        free(groups[i].events);
    }
    free(groups);

    return 0;
}