// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *desc; } Event;
typedef struct { char *name; Event *events; int ev_count, ev_cap; } Group;
Group *groups = NULL;
int group_count = 0, group_cap = 0;

int find_group(const char *name) {
    for (int i = 0; i < group_count; i++)
        if (strcmp(groups[i].name, name) == 0) return i;
    return -1;
}

void add_event(const char *group, const char *desc) {
    int idx = find_group(group);
    if (idx < 0) {
        if (group_count == group_cap) {
            group_cap = group_cap ? group_cap * 2 : 4;
            groups = realloc(groups, sizeof(Group) * group_cap);
        }
        groups[group_count].name = strdup(group);
        groups[group_count].events = NULL;
        groups[group_count].ev_count = 0;
        groups[group_count].ev_cap = 0;
        idx = group_count++;
    }
    Group *g = &groups[idx];
    if (g->ev_count == g->ev_cap) {
        g->ev_cap = g->ev_cap ? g->ev_cap * 2 : 4;
        g->events = realloc(g->events, sizeof(Event) * g->ev_cap);
    }
    g->events[g->ev_count].desc = strdup(desc);
    g->ev_count++;
}

void merge_groups(const char *a, const char *b, const char *newg) {
    int ia = find_group(a), ib = find_group(b);
    if (ia < 0 || ib < 0) return;
    if (find_group(newg) >= 0) return;
    if (group_count == group_cap) {
        group_cap = group_cap ? group_cap * 2 : 4;
        groups = realloc(groups, sizeof(Group) * group_cap);
    }
    groups[group_count].name = strdup(newg);
    groups[group_count].events = NULL;
    groups[group_count].ev_count = 0;
    groups[group_count].ev_cap = 0;
    Group *ng = &groups[group_count++];
    for (int i = 0; i < groups[ia].ev_count; i++)
        add_event(newg, groups[ia].events[i].desc);
    for (int i = 0; i < groups[ib].ev_count; i++)
        add_event(newg, groups[ib].events[i].desc);
}

void delete_group(const char *name) {
    int idx = find_group(name);
    if (idx < 0) return;
    for (int i = 0; i < groups[idx].ev_count; i++) free(groups[idx].events[i].desc);
    free(groups[idx].events);
    free(groups[idx].name);
    for (int i = idx; i < group_count - 1; i++) groups[i] = groups[i + 1];
    group_count--;
}

void count_group(const char *name) {
    int idx = find_group(name);
    printf("%d\n", idx >= 0 ? groups[idx].ev_count : 0);
}

void report_groups(void) {
    for (int i = 0; i < group_count; i++)
        printf("%s %d\n", groups[i].name, groups[i].ev_count);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], arg3[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "EVENT") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, arg3) == 2)
                add_event(arg1, arg3);
        } else if (strcmp(cmd, "MERGE") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", arg1, arg2, arg3) == 3)
                merge_groups(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) delete_group(arg1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) count_group(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) report_groups();
    }
    for (int i = 0; i < group_count; i++) {
        for (int j = 0; j < groups[i].ev_count; j++) free(groups[i].events[j].desc);
        free(groups[i].events);
        free(groups[i].name);
    }
    free(groups);
    return 0;
}