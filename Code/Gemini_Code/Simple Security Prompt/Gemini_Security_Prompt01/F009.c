// F009.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *group;
    char *desc;
} Event;

int main() {
    Event *events = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "EVENT") == 0 && args) {
            char *group = args;
            char *space2 = strchr(args, ' ');
            char *desc = "";
            if (space2) {
                *space2 = '\0';
                desc = space2 + 1;
                while (*desc == ' ') desc++;
            }
            events = realloc(events, (count + 1) * sizeof(Event));
            if (!events) return 1;
            events[count].group = strdup(group);
            events[count].desc = strdup(desc);
            count++;
        } else if (strcmp(cmd, "MERGE") == 0 && args) {
            char *gA = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *gB = space2 + 1;
                while (*gB == ' ') gB++;
                char *space3 = strchr(gB, ' ');
                if (space3) {
                    *space3 = '\0';
                    char *newG = space3 + 1;
                    while (*newG == ' ') newG++;

                    int current_count = count;
                    for (int i = 0; i < current_count; i++) {
                        if (strcmp(events[i].group, gA) == 0 || strcmp(events[i].group, gB) == 0) {
                            events = realloc(events, (count + 1) * sizeof(Event));
                            if (!events) return 1;
                            events[count].group = strdup(newG);
                            events[count].desc = strdup(events[i].desc);
                            count++;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0 && args) {
            int idx = 0;
            while (idx < count) {
                if (strcmp(events[idx].group, args) == 0) {
                    free(events[idx].group);
                    free(events[idx].desc);
                    for (int j = idx; j < count - 1; j++) {
                        events[j] = events[j + 1];
                    }
                    count--;
                } else {
                    idx++;
                }
            }
            events = realloc(events, count * sizeof(Event));
            if (count > 0 && !events) return 1;
        } else if (strcmp(cmd, "COUNT") == 0 && args) {
            int c = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(events[i].group, args) == 0) {
                    c++;
                }
            }
            printf("%d\n", c);
        } else if (strcmp(cmd, "REPORT") == 0) {
            char **seen = NULL;
            int seen_count = 0;
            for (int i = 0; i < count; i++) {
                int found = 0;
                for (int j = 0; j < seen_count; j++) {
                    if (strcmp(seen[j], events[i].group) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    printf("%s\n", events[i].group);
                    seen = realloc(seen, (seen_count + 1) * sizeof(char *));
                    if (!seen) return 1;
                    seen[seen_count] = events[i].group;
                    seen_count++;
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
