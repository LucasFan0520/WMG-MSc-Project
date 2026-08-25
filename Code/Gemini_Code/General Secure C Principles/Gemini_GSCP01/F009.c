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
    size_t event_count = 0;
    char **groups = NULL;
    size_t group_count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "EVENT") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *grp = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *desc = p;
            int g_found = 0;
            for (size_t i = 0; i < group_count; i++) {
                if (strcmp(groups[i], grp) == 0) {
                    g_found = 1;
                    break;
                }
            }
            if (!g_found) {
                char **temp = realloc(groups, (group_count + 1) * sizeof(char *));
                if (temp) {
                    groups = temp;
                    groups[group_count] = strdup(grp);
                    group_count++;
                }
            }
            Event *temp_e = realloc(events, (event_count + 1) * sizeof(Event));
            if (temp_e) {
                events = temp_e;
                events[event_count].group = strdup(grp);
                events[event_count].description = strdup(desc);
                event_count++;
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *grpA = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *grpB = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *newGrp = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int g_found = 0;
            for (size_t i = 0; i < group_count; i++) {
                if (strcmp(groups[i], newGrp) == 0) {
                    g_found = 1;
                    break;
                }
            }
            if (!g_found) {
                char **temp = realloc(groups, (group_count + 1) * sizeof(char *));
                if (temp) {
                    groups = temp;
                    groups[group_count] = strdup(newGrp);
                    group_count++;
                }
            }
            size_t current_limit = event_count;
            for (size_t i = 0; i < current_limit; i++) {
                if (strcmp(events[i].group, grpA) == 0 || strcmp(events[i].group, grpB) == 0) {
                    Event *temp_e = realloc(events, (event_count + 1) * sizeof(Event));
                    if (temp_e) {
                        events = temp_e;
                        events[event_count].group = strdup(newGrp);
                        events[event_count].description = strdup(events[i].description);
                        event_count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *grp = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            size_t i = 0;
            while (i < event_count) {
                if (strcmp(events[i].group, grp) == 0) {
                    free(events[i].group);
                    free(events[i].description);
                    for (size_t j = i; j < event_count - 1; j++) {
                        events[j] = events[j + 1];
                    }
                    event_count--;
                } else {
                    i++;
                }
            }
            for (size_t k = 0; k < group_count; k++) {
                if (strcmp(groups[k], grp) == 0) {
                    free(groups[k]);
                    for (size_t j = k; j < group_count - 1; j++) {
                        groups[j] = groups[j + 1];
                    }
                    group_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *grp = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int cnt = 0;
            for (size_t i = 0; i < event_count; i++) {
                if (strcmp(events[i].group, grp) == 0) {
                    cnt++;
                }
            }
            printf("%d\n", cnt);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < group_count; i++) {
                printf("%s\n", groups[i]);
            }
        }
    }
    for (size_t i = 0; i < event_count; i++) {
        free(events[i].group);
        free(events[i].description);
    }
    free(events);
    for (size_t i = 0; i < group_count; i++) {
        free(groups[i]);
    }
    free(groups);
    free(line);
    return 0;
}
