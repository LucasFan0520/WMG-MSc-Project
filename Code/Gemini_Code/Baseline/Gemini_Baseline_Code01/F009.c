// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *description;
} Event;

typedef struct {
    char *name;
    Event *events;
    int event_count;
} Group;

int main() {
    Group *groups = NULL;
    int group_count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "EVENT") == 0) {
            char gname[256];
            int gbytes = 0;
            if (sscanf(args, "%255s%n", gname, &gbytes) == 1) {
                char *desc = args + gbytes;
                while (*desc == ' ') desc++;
                int found = -1;
                for (int i = 0; i < group_count; i++) {
                    if (strcmp(groups[i].name, gname) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    groups = realloc(groups, (group_count + 1) * sizeof(Group));
                    groups[group_count].name = strdup(gname);
                    groups[group_count].events = NULL;
                    groups[group_count].event_count = 0;
                    found = group_count;
                    group_count++;
                }
                groups[found].events = realloc(groups[found].events, (groups[found].event_count + 1) * sizeof(Event));
                groups[found].events[groups[found].event_count].description = strdup(desc);
                groups[found].event_count++;
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char gA[256], gB[256], gNew[256];
            if (sscanf(args, "%255s %255s %255s", gA, gB, gNew) == 3) {
                int idxA = -1, idxB = -1;
                for (int i = 0; i < group_count; i++) {
                    if (strcmp(groups[i].name, gA) == 0) idxA = i;
                    if (strcmp(groups[i].name, gB) == 0) idxB = i;
                }
                if (idxA != -1 && idxB != -1) {
                    groups = realloc(groups, (group_count + 1) * sizeof(Group));
                    groups[group_count].name = strdup(gNew);
                    groups[group_count].event_count = groups[idxA].event_count + groups[idxB].event_count;
                    groups[group_count].events = malloc(groups[group_count].event_count * sizeof(Event));
                    int e_idx = 0;
                    for (int i = 0; i < groups[idxA].event_count; i++) {
                        groups[group_count].events[e_idx++].description = strdup(groups[idxA].events[i].description);
                    }
                    for (int i = 0; i < groups[idxB].event_count; i++) {
                        groups[group_count].events[e_idx++].description = strdup(groups[idxB].events[i].description);
                    }
                    group_count++;
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char gname[256];
            if (sscanf(args, "%255s", gname) == 1) {
                for (int i = 0; i < group_count; i++) {
                    if (strcmp(groups[i].name, gname) == 0) {
                        free(groups[i].name);
                        for (int j = 0; j < groups[i].event_count; j++) {
                            free(groups[i].events[j].description);
                        }
                        free(groups[i].events);
                        for (int j = i; j < group_count - 1; j++) {
                            groups[j] = groups[j + 1];
                        }
                        group_count--;
                        groups = realloc(groups, group_count * sizeof(Group));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char gname[256];
            if (sscanf(args, "%255s", gname) == 1) {
                int c = 0;
                for (int i = 0; i < group_count; i++) {
                    if (strcmp(groups[i].name, gname) == 0) {
                        c = groups[i].event_count;
                        break;
                    }
                }
                printf("%d\n", c);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < group_count; i++) {
                printf("%s %d\n", groups[i].name, groups[i].event_count);
            }
        }
    }
    for (int i = 0; i < group_count; i++) {
        free(groups[i].name);
        for (int j = 0; j < groups[i].event_count; j++) {
            free(groups[i].events[j].description);
        }
        free(groups[i].events);
    }
    free(groups);
    return 0;
}
