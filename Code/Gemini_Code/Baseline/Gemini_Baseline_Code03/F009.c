// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *group;
    char *description;
} Event;

int main() {
    Event *events = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "EVENT") == 0) {
            char group[256];
            int g_len = 0;
            if (sscanf(ptr, "%255s%n", group, &g_len) > 0) {
                char *desc = ptr + g_len;
                while (*desc == ' ') desc++;
                events = realloc(events, (count + 1) * sizeof(Event));
                events[count].group = strdup(group);
                events[count].description = strdup(desc);
                count++;
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char groupA[256], groupB[256], newGroup[256];
            if (sscanf(ptr, "%255s %255s %255s", groupA, groupB, newGroup) == 3) {
                int original_count = count;
                for (int i = 0; i < original_count; i++) {
                    if (strcmp(events[i].group, groupA) == 0 || strcmp(events[i].group, groupB) == 0) {
                        events = realloc(events, (count + 1) * sizeof(Event));
                        events[count].group = strdup(newGroup);
                        events[count].description = strdup(events[i].description);
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char group[256];
            if (sscanf(ptr, "%255s", group) > 0) {
                int i = 0;
                while (i < count) {
                    if (strcmp(events[i].group, group) == 0) {
                        free(events[i].group);
                        free(events[i].description);
                        for (int j = i; j < count - 1; j++) {
                            events[j] = events[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
                if (count > 0) {
                    events = realloc(events, count * sizeof(Event));
                } else {
                    free(events);
                    events = NULL;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char group[256];
            if (sscanf(ptr, "%255s", group) > 0) {
                int g_count = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(events[i].group, group) == 0) {
                        g_count++;
                    }
                }
                printf("%d\n", g_count);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            char **printed = NULL;
            int p_count = 0;
            for (int i = 0; i < count; i++) {
                int already_printed = 0;
                for (int j = 0; j < p_count; j++) {
                    if (strcmp(printed[j], events[i].group) == 0) {
                        already_printed = 1;
                        break;
                    }
                }
                if (!already_printed) {
                    printf("%s\n", events[i].group);
                    printed = realloc(printed, (p_count + 1) * sizeof(char *));
                    printed[p_count] = events[i].group;
                    p_count++;
                }
            }
            free(printed);
        }
    }
    for (int i = 0; i < count; i++) {
        free(events[i].group);
        free(events[i].description);
    }
    free(events);
    return 0;
}
