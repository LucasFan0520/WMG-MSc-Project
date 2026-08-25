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
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "EVENT ", 6) == 0) {
            char *args = line + 6;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *group = args;
                char *description = space + 1;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Event *next = realloc(events, capacity * sizeof(Event));
                    if (!next) break;
                    events = next;
                }
                events[count].group = strdup(group);
                events[count].description = strdup(description);
                count++;
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *args = line + 6;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *groupA = args;
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    char *groupB = rest;
                    char *newGroup = sp2 + 1;
                    size_t orig_count = count;
                    for (size_t i = 0; i < orig_count; i++) {
                        if (strcmp(events[i].group, groupA) == 0 || strcmp(events[i].group, groupB) == 0) {
                            if (count >= capacity) {
                                capacity = capacity == 0 ? 4 : capacity * 2;
                                Event *next = realloc(events, capacity * sizeof(Event));
                                if (!next) break;
                                events = next;
                            }
                            events[count].group = strdup(newGroup);
                            events[count].description = strdup(events[i].description);
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETEGROUP ", 12) == 0) {
            char *group = line + 12;
            size_t i = 0;
            while (i < count) {
                if (strcmp(events[i].group, group) == 0) {
                    free(events[i].group);
                    free(events[i].description);
                    for (size_t j = i; j < count - 1; j++) {
                        events[j] = events[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *group = line + 6;
            size_t amt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(events[i].group, group) == 0) {
                    amt++;
                }
            }
            printf("%zu\n", amt);
        } else if (strcmp(line, "REPORT") == 0) {
            char **seen = NULL;
            size_t seen_count = 0;
            for (size_t i = 0; i < count; i++) {
                int found = 0;
                for (size_t j = 0; j < seen_count; j++) {
                    if (strcmp(seen[j], events[i].group) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    printf("%s\n", events[i].group);
                    seen = realloc(seen, (seen_count + 1) * sizeof(char *));
                    seen[seen_count++] = events[i].group;
                }
            }
            free(seen);
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(events[i].group);
        free(events[i].description);
    }
    free(events);
    return 0;
}
