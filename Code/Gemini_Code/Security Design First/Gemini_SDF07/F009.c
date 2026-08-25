/* F009.c */
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
    int capacity = 0;
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
            char *grp = NULL;
            char *description = NULL;
            if (space) {
                *space = '\0';
                grp = strdup(args);
                description = strdup(space + 1);
                *space = ' ';
            } else {
                grp = strdup(args);
                description = strdup("");
            }
            if (!grp || !description) {
                free(grp);
                free(description);
                continue;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Event *tmp = realloc(events, new_cap * sizeof(Event));
                if (tmp) {
                    events = tmp;
                    capacity = new_cap;
                } else {
                    free(grp);
                    free(description);
                    continue;
                }
            }
            events[count].group = grp;
            events[count].desc = description;
            count++;
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *args = line + 6;
            char *space1 = strchr(args, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *grpA = args;
            char *rest = space1 + 1;
            char *space2 = strchr(rest, ' ');
            if (!space2) {
                *space1 = ' ';
                continue;
            }
            *space2 = '\0';
            char *grpB = rest;
            char *newGrp = space2 + 1;

            int old_count = count;
            for (int i = 0; i < old_count; i++) {
                if (strcmp(events[i].group, grpA) == 0 || strcmp(events[i].group, grpB) == 0) {
                    char *n_grp = strdup(newGrp);
                    char *n_desc = strdup(events[i].desc);
                    if (n_grp && n_desc) {
                        if (count >= capacity) {
                            int new_cap = capacity == 0 ? 4 : capacity * 2;
                            Event *tmp = realloc(events, new_cap * sizeof(Event));
                            if (tmp) {
                                events = tmp;
                                capacity = new_cap;
                            } else {
                                free(n_grp);
                                free(n_desc);
                                break;
                            }
                        }
                        events[count].group = n_grp;
                        events[count].desc = n_desc;
                        count++;
                    } else {
                        free(n_grp);
                        free(n_desc);
                    }
                }
            }
            *space1 = ' ';
            *space2 = ' ';
        } else if (strncmp(line, "DELETEGROUP ", 12) == 0) {
            char *grp = line + 12;
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
            int c = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(events[i].group, grp) == 0) {
                    c++;
                }
            }
            printf("%d\n", c);
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
                    printf("%s\n", events[i].group);
                    char **tmp = realloc(seen, (seen_count + 1) * sizeof(char *));
                    if (tmp) {
                        seen = tmp;
                        seen[seen_count] = events[i].group;
                        seen_count++;
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
