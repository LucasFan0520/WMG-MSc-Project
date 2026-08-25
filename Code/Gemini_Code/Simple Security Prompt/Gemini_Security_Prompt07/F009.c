// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *desc;
} Event;

typedef struct {
    char *name;
    Event *events;
    int event_count;
} Group;

int main() {
    Group *groups = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "EVENT") == 0) {
            char *gname = strtok(NULL, " ");
            char *desc = strtok(NULL, "");
            if (gname && desc) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(groups[i].name, gname) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    groups = realloc(groups, (count + 1) * sizeof(Group));
                    groups[count].name = strdup(gname);
                    groups[count].events = NULL;
                    groups[count].event_count = 0;
                    found = count;
                    count++;
                }
                groups[found].events = realloc(groups[found].events, (groups[found].event_count + 1) * sizeof(Event));
                groups[found].events[groups[found].event_count].desc = strdup(desc);
                groups[found].event_count++;
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *gA = strtok(NULL, " ");
            char *gB = strtok(NULL, " ");
            char *gNew = strtok(NULL, " ");
            if (gA && gB && gNew) {
                int idxA = -1, idxB = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(groups[i].name, gA) == 0) idxA = i;
                    if (strcmp(groups[i].name, gB) == 0) idxB = i;
                }
                groups = realloc(groups, (count + 1) * sizeof(Group));
                groups[count].name = strdup(gNew);
                groups[count].events = NULL;
                groups[count].event_count = 0;
                if (idxA != -1) {
                    for (int i = 0; i < groups[idxA].event_count; i++) {
                        groups[count].events = realloc(groups[count].events, (groups[count].event_count + 1) * sizeof(Event));
                        groups[count].events[groups[count].event_count].desc = strdup(groups[idxA].events[i].desc);
                        groups[count].event_count++;
                    }
                }
                if (idxB != -1) {
                    for (int i = 0; i < groups[idxB].event_count; i++) {
                        groups[count].events = realloc(groups[count].events, (groups[count].event_count + 1) * sizeof(Event));
                        groups[count].events[groups[count].event_count].desc = strdup(groups[idxB].events[i].desc);
                        groups[count].event_count++;
                    }
                }
                count++;
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *gname = strtok(NULL, " ");
            if (gname) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(groups[i].name, gname) == 0) {
                        free(groups[i].name);
                        for (int j = 0; j < groups[i].event_count; j++) {
                            free(groups[i].events[j].desc);
                        }
                        free(groups[i].events);
                        for (int j = i; j < count - 1; j++) {
                            groups[j] = groups[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(groups);
                    groups = NULL;
                } else {
                    groups = realloc(groups, count * sizeof(Group));
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *gname = strtok(NULL, " ");
            if (gname) {
                int cnt = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(groups[i].name, gname) == 0) {
                        cnt = groups[i].event_count;
                        break;
                    }
                }
                printf("%d\n", cnt);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", groups[i].name, groups[i].event_count);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(groups[i].name);
        for (int j = 0; j < groups[i].event_count; j++) {
            free(groups[i].events[j].desc);
        }
        free(groups[i].events);
    }
    free(groups);
    return 0;
}
