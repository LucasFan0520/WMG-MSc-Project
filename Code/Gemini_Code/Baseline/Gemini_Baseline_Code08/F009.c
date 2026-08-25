// F009.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char **events;
    int event_count;
    int event_capacity;
    char *name;
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

int create_group(const char *name) {
    int idx = find_group(name);
    if (idx != -1) return idx;
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

void add_event(const char *g_name, const char *desc) {
    int idx = create_group(g_name);
    Group *g = &groups[idx];
    if (g->event_count >= g->event_capacity) {
        g->event_capacity = g->event_capacity == 0 ? 4 : g->event_capacity * 2;
        g->events = realloc(g->events, g->event_capacity * sizeof(char *));
    }
    g->events[g->event_count] = strdup(desc);
    g->event_count++;
}

void merge_groups(const char *gA, const char *gB, const char *gNew) {
    int idxA = find_group(gA);
    int idxB = find_group(gB);
    int idxNew = create_group(gNew);
    if (idxA != -1) {
        for (int i = 0; i < groups[idxA].event_count; i++) {
            add_event(gNew, groups[idxA].events[i]);
        }
    }
    if (idxB != -1) {
        for (int i = 0; i < groups[idxB].event_count; i++) {
            add_event(gNew, groups[idxB].events[i]);
        }
    }
}

void delete_group(const char *name) {
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
}

void count_group(const char *name) {
    int idx = find_group(name);
    if (idx != -1) {
        printf("%d\n", groups[idx].event_count);
    } else {
        printf("0\n");
    }
}

void report_groups(void) {
    for (int i = 0; i < group_count; i++) {
        printf("%s %d\n", groups[i].name, groups[i].event_count);
    }
}

int main(void) {
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
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "EVENT") == 0) {
            char *g_name = strtok(NULL, " ");
            char *desc = strtok(NULL, "");
            if (g_name && desc) add_event(g_name, desc);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *gA = strtok(NULL, " ");
            char *gB = strtok(NULL, " ");
            char *gNew = strtok(NULL, " ");
            if (gA && gB && gNew) merge_groups(gA, gB, gNew);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_group(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *name = strtok(NULL, " ");
            if (name) count_group(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    free(line);
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
