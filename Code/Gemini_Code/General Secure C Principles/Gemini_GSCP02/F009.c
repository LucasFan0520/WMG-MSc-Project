// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *group;
    char *desc;
} Event;

int main(void) {
    Event *events = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    ssize_t read;
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "EVENT ", 6) == 0) {
            char grp[256];
            char desc[1024];
            if (sscanf(line + 6, "%255s %[^\n]", grp, desc) == 2) {
                Event *tmp = realloc(events, (count + 1) * sizeof(Event));
                if (tmp) {
                    events = tmp;
                    events[count].group = strdup(grp);
                    events[count].desc = strdup(desc);
                    count++;
                }
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char gA[256], gB[256], nG[256];
            if (sscanf(line + 6, "%255s %255s %255s", gA, gB, nG) == 3) {
                int old_count = count;
                for (int i = 0; i < old_count; i++) {
                    if (strcmp(events[i].group, gA) == 0 || strcmp(events[i].group, gB) == 0) {
                        Event *tmp = realloc(events, (count + 1) * sizeof(Event));
                        if (tmp) {
                            events = tmp;
                            events[count].group = strdup(nG);
                            events[count].desc = strdup(events[i].desc);
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETEGROUP ", 12) == 0) {
            char *grp = line + 12;
            while (*grp == ' ') {
                grp++;
            }
            int i = 0;
            while (i < count) {
                if (strcmp(events[i].group, grp) == 0) {
                    free(events[i].group);
                    free(events[i].desc);
                    for (int j = i; j < count - 1; j++) {
                        events[j] = events[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *grp = line + 6;
            while (*grp == ' ') {
                grp++;
            }
            int ans = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(events[i].group, grp) == 0) {
                    ans++;
                }
            }
            printf("%d\n", ans);
        } else if (strcmp(line, "REPORT") == 0) {
            char **seen = NULL;
            int scount = 0;
            for (int i = 0; i < count; i++) {
                int found = 0;
                for (int j = 0; j < scount; j++) {
                    if (strcmp(seen[j], events[i].group) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    printf("%s\n", events[i].group);
                    char **tmp = realloc(seen, (scount + 1) * sizeof(char *));
                    if (tmp) {
                        seen = tmp;
                        seen[scount] = events[i].group;
                        scount++;
                    }
                }
            }
            free(seen);
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(events[i].group);
        free(events[i].desc);
    }
    free(events);
    return 0;
}
