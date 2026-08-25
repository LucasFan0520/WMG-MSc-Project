// F009.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *group;
    char *description;
} Event;

int main(void) {
    Event *events = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "EVENT ", 6) == 0) {
            char *p = line + 6;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *group = p;
                char *desc = space + 1;
                Event *tmp = realloc(events, sizeof(Event) * (count + 1));
                if (tmp) {
                    events = tmp;
                    events[count].group = strdup(group);
                    events[count].description = strdup(desc);
                    if (events[count].group && events[count].description) {
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *p = line + 6;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *gA = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *gB = p2;
                    char *newG = s2 + 1;
                    int old_count = count;
                    for (int i = 0; i < old_count; i++) {
                        if (strcmp(events[i].group, gA) == 0 || strcmp(events[i].group, gB) == 0) {
                            Event *tmp = realloc(events, sizeof(Event) * (count + 1));
                            if (tmp) {
                                events = tmp;
                                events[count].group = strdup(newG);
                                events[count].description = strdup(events[i].description);
                                if (events[count].group && events[count].description) {
                                    count++;
                                }
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETEGROUP ", 12) == 0) {
            char *group = line + 12;
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
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *group = line + 6;
            int cnt = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(events[i].group, group) == 0) {
                    cnt++;
                }
            }
            printf("%d\n", cnt);
        } else if (strcmp(line, "REPORT") == 0) {
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
                    char **tmp = realloc(seen, sizeof(char *) * (seen_count + 1));
                    if (tmp) {
                        seen = tmp;
                        seen[seen_count] = events[i].group;
                        seen_count++;
                        printf("%s\n", events[i].group);
                    }
                }
            }
            free(seen);
        }
    }
    for (int i = 0; i < count; i++) {
        free(events[i].group);
        free(events[i].description);
    }
    free(events);
    free(line);
    return 0;
}
