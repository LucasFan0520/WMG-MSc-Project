// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char **events;
    int event_count;
    int event_capacity;
} Group;

Group *groups = NULL;
int group_count = 0;
int group_capacity = 0;

int find_group(const char *name) {
    for (int i = 0; i < group_count; i++) {
        if (strcmp(groups[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int add_group(const char *name) {
    int idx = find_group(name);
    if (idx != -1) {
        return idx;
    }
    if (group_count >= group_capacity) {
        group_capacity = group_capacity == 0 ? 4 : group_capacity * 2;
        groups = realloc(groups, group_capacity * sizeof(Group));
    }
    groups[group_count].name = strdup(name);
    groups[group_count].events = NULL;
    groups[group_count].event_count = 0;
    groups[group_count].event_capacity = 0;
    group_count++;
    return group_count - 1;
}

void add_event(int g_idx, const char *desc) {
    Group *g = &groups[g_idx];
    if (g->event_count >= g->event_capacity) {
        g->event_capacity = g->event_capacity == 0 ? 4 : g->event_capacity * 2;
        g->events = realloc(g->events, g->event_capacity * sizeof(char *));
    }
    g->events[g->event_count] = strdup(desc);
    g->event_count++;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "EVENT ", 6) == 0) {
            char *p1 = line + 6;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                int g_idx = add_group(p1);
                add_event(g_idx, p2 + 1);
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *p1 = line + 6;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                char *p3 = strchr(p2 + 1, ' ');
                if (p3) {
                    *p3 = 0;
                    int idxA = find_group(p1);
                    int idxB = find_group(p2 + 1);
                    int idxNew = add_group(p3 + 1);
                    if (idxA != -1) {
                        for (int i = 0; i < groups[idxA].event_count; i++) {
                            add_event(idxNew, groups[idxA].events[i]);
                        }
                    }
                    if (idxB != -1) {
                        for (int i = 0; i < groups[idxB].event_count; i++) {
                            add_event(idxNew, groups[idxB].events[i]);
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETEGROUP ", 12) == 0) {
            char *name = line + 12;
            int idx = find_group(name);
            if (idx != -1) {
                free(groups[idx].name);
                for (int i = 0; i < groups[idx].event_count; i++) {
                    free(groups[idx].events[i]);
                }
                free(groups[idx].events);
                for (int i = idx; i < group_count - 1; i++) {
                    groups[i] = groups[i + 1];
                }
                group_count--;
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *name = line + 6;
            int idx = find_group(name);
            if (idx != -1) {
                printf("%d\n", groups[idx].event_count);
            } else {
                printf("0\n");
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < group_count; i++) {
                printf("%s %d\n", groups[i].name, groups[i].event_count);
            }
        }
    }
    for (int i = 0; i < group_count; i++) {
        free(groups[i].name);
        for (int j = 0; j < groups[i].event_count; j++) {
            free(groups[i].events[j]);
        }
        free(groups[i].events);
    }
    free(groups);
    return 0;
}
