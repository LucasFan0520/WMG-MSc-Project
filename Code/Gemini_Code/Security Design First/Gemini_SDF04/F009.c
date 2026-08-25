// F009.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *group;
    char *description;
} Event;

Event *events = NULL;
int count = 0;
int capacity = 0;

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "EVENT") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *desc = space2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Event *tmp = realloc(events, capacity * sizeof(Event));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        events = tmp;
                    }
                    events[count].group = strdup(arg1);
                    events[count].description = strdup(desc);
                    if (events[count].group && events[count].description) {
                        count++;
                    }
                }
            } else if (strcmp(cmd, "MERGE") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *arg2 = space2 + 1;
                    char *space3 = strchr(arg2, ' ');
                    if (space3) {
                        *space3 = '\0';
                        char *newG = space3 + 1;
                        int orig_count = count;
                        for (int i = 0; i < orig_count; i++) {
                            if (strcmp(events[i].group, arg1) == 0 || strcmp(events[i].group, arg2) == 0) {
                                if (count >= capacity) {
                                    capacity = capacity == 0 ? 4 : capacity * 2;
                                    Event *tmp = realloc(events, capacity * sizeof(Event));
                                    if (!tmp) {
                                        free(line);
                                        return 1;
                                    }
                                    events = tmp;
                                }
                                events[count].group = strdup(newG);
                                events[count].description = strdup(events[i].description);
                                if (events[count].group && events[count].description) {
                                    count++;
                                }
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "DELETEGROUP") == 0) {
                for (int i = 0; i < count; ) {
                    if (strcmp(events[i].group, arg1) == 0) {
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
            } else if (strcmp(cmd, "COUNT") == 0) {
                int c = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(events[i].group, arg1) == 0) {
                        c++;
                    }
                }
                printf("%d\n", c);
            }
        } else {
            if (strcmp(cmd, "REPORT") == 0) {
                char **printed = malloc(count * sizeof(char*));
                int p_count = 0;
                for (int i = 0; i < count; i++) {
                    int found = 0;
                    for (int j = 0; j < p_count; j++) {
                        if (strcmp(printed[j], events[i].group) == 0) {
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        printf("%s\n", events[i].group);
                        if (printed) {
                            printed[p_count++] = events[i].group;
                        }
                    }
                }
                free(printed);
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(events[i].group);
        free(events[i].description);
    }
    free(events);
    return 0;
}