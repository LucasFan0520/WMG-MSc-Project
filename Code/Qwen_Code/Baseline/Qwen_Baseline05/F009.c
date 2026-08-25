// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_GROUP 100
#define MAX_DESC 1000

typedef struct {
    char desc[MAX_DESC];
} Event;

typedef struct {
    char name[MAX_GROUP];
    Event events[1000];
    int event_count;
} Group;

Group groups[1000];
int group_count = 0;

int find_group(const char *name) {
    for (int i = 0; i < group_count; i++) {
        if (strcmp(groups[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20], group[MAX_GROUP], groupA[MAX_GROUP], groupB[MAX_GROUP], newGroup[MAX_GROUP], desc[MAX_DESC];
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "EVENT") == 0) {
            scanf("%s", group);
            fgets(desc, MAX_DESC, stdin);
            if (desc[0] == ' ') memmove(desc, desc + 1, strlen(desc));
            if (desc[strlen(desc) - 1] == '\n') desc[strlen(desc) - 1] = '\0';
            
            int idx = find_group(group);
            if (idx == -1) {
                strcpy(groups[group_count].name, group);
                groups[group_count].event_count = 0;
                idx = group_count++;
            }
            strcpy(groups[idx].events[groups[idx].event_count].desc, desc);
            groups[idx].event_count++;
        } else if (strcmp(cmd, "MERGE") == 0) {
            scanf("%s %s %s", groupA, groupB, newGroup);
            int idxA = find_group(groupA);
            int idxB = find_group(groupB);
            
            int idxNew = find_group(newGroup);
            if (idxNew == -1) {
                strcpy(groups[group_count].name, newGroup);
                groups[group_count].event_count = 0;
                idxNew = group_count++;
            }
            
            if (idxA != -1) {
                for (int i = 0; i < groups[idxA].event_count; i++) {
                    strcpy(groups[idxNew].events[groups[idxNew].event_count].desc, groups[idxA].events[i].desc);
                    groups[idxNew].event_count++;
                }
            }
            if (idxB != -1) {
                for (int i = 0; i < groups[idxB].event_count; i++) {
                    strcpy(groups[idxNew].events[groups[idxNew].event_count].desc, groups[idxB].events[i].desc);
                    groups[idxNew].event_count++;
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            scanf("%s", group);
            int idx = find_group(group);
            if (idx != -1) {
                for (int i = idx; i < group_count - 1; i++) {
                    groups[i] = groups[i + 1];
                }
                group_count--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            scanf("%s", group);
            int idx = find_group(group);
            if (idx != -1) {
                printf("%d\n", groups[idx].event_count);
            } else {
                printf("0\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < group_count; i++) {
                printf("%s\n", groups[i].name);
                for (int j = 0; j < groups[i].event_count; j++) {
                    printf("  %s\n", groups[i].events[j].desc);
                }
            }
        }
    }
    
    return 0;
}