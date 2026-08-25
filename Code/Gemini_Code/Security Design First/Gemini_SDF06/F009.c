// F009.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char **descriptions;
    size_t desc_count;
    size_t desc_capacity;
} Group;

int main(void) {
    Group *groups = NULL;
    size_t group_count = 0;
    size_t group_capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "EVENT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *group_name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *description = p;

            if (*group_name == '\0') continue;

            int found_idx = -1;
            for (size_t i = 0; i < group_count; i++) {
                if (strcmp(groups[i].name, group_name) == 0) {
                    found_idx = (int)i;
                    break;
                }
            }
            if (found_idx == -1) {
                if (group_count >= group_capacity) {
                    size_t new_cap = group_capacity == 0 ? 4 : group_capacity * 2;
                    Group *new_groups = realloc(groups, new_cap * sizeof(Group));
                    if (!new_groups) continue;
                    groups = new_groups;
                    group_capacity = new_cap;
                }
                char *gn = strdup(group_name);
                if (gn) {
                    groups[group_count].name = gn;
                    groups[group_count].descriptions = NULL;
                    groups[group_count].desc_count = 0;
                    groups[group_count].desc_capacity = 0;
                    found_idx = (int)group_count;
                    group_count++;
                }
            }
            if (found_idx != -1) {
                Group *g = &groups[found_idx];
                if (g->desc_count >= g->desc_capacity) {
                    size_t new_cap = g->desc_capacity == 0 ? 4 : g->desc_capacity * 2;
                    char **new_desc = realloc(g->descriptions, new_cap * sizeof(char *));
                    if (!new_desc) continue;
                    g->descriptions = new_desc;
                    g->desc_capacity = new_cap;
                }
                char *d = strdup(description);
                if (d) {
                    g->descriptions[g->desc_count] = d;
                    g->desc_count++;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *groupA = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *groupB = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *newGroup = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*groupA == '\0' || *groupB == '\0' || *newGroup == '\0') continue;

            int idxA = -1, idxB = -1, idxNew = -1;
            for (size_t i = 0; i < group_count; i++) {
                if (strcmp(groups[i].name, groupA) == 0) idxA = (int)i;
                if (strcmp(groups[i].name, groupB) == 0) idxB = (int)i;
                if (strcmp(groups[i].name, newGroup) == 0) idxNew = (int)i;
            }
            if (idxA != -1 && idxB != -1) {
                if (idxNew == -1) {
                    if (group_count >= group_capacity) {
                        size_t new_cap = group_capacity == 0 ? 4 : group_capacity * 2;
                        Group *new_groups = realloc(groups, new_cap * sizeof(Group));
                        if (!new_groups) continue;
                        groups = new_groups;
                        group_capacity = new_cap;
                    }
                    char *gn = strdup(newGroup);
                    if (gn) {
                        groups[group_count].name = gn;
                        groups[group_count].descriptions = NULL;
                        groups[group_count].desc_count = 0;
                        groups[group_count].desc_capacity = 0;
                        idxNew = (int)group_count;
                        group_count++;
                    }
                } else {
                    Group *gNew = &groups[idxNew];
                    for (size_t i = 0; i < gNew->desc_count; i++) {
                        free(gNew->descriptions[i]);
                    }
                    free(gNew->descriptions);
                    gNew->descriptions = NULL;
                    gNew->desc_count = 0;
                    gNew->desc_capacity = 0;
                }
                if (idxNew != -1) {
                    Group *gA = &groups[idxA];
                    Group *gB = &groups[idxB];
                    Group *gNew = &groups[idxNew];
                    size_t total_needed = gA->desc_count + gB->desc_count;
                    if (total_needed > 0) {
                        gNew->descriptions = malloc(total_needed * sizeof(char *));
                        if (gNew->descriptions) {
                            gNew->desc_capacity = total_needed;
                            for (size_t i = 0; i < gA->desc_count; i++) {
                                char *d = strdup(gA->descriptions[i]);
                                if (d) {
                                    gNew->descriptions[gNew->desc_count++] = d;
                                }
                            }
                            for (size_t i = 0; i < gB->desc_count; i++) {
                                char *d = strdup(gB->descriptions[i]);
                                if (d) {
                                    gNew->descriptions[gNew->desc_count++] = d;
                                }
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *group_name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*group_name == '\0') continue;

            for (size_t i = 0; i < group_count; i++) {
                if (strcmp(groups[i].name, group_name) == 0) {
                    free(groups[i].name);
                    for (size_t j = 0; j < groups[i].desc_count; j++) {
                        free(groups[i].descriptions[j]);
                    }
                    free(groups[i].descriptions);
                    for (size_t j = i; j < group_count - 1; j++) {
                        groups[j] = groups[j + 1];
                    }
                    group_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *group_name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*group_name == '\0') continue;

            size_t cnt = 0;
            for (size_t i = 0; i < group_count; i++) {
                if (strcmp(groups[i].name, group_name) == 0) {
                    cnt = groups[i].desc_count;
                    break;
                }
            }
            printf("%zu\n", cnt);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < group_count; i++) {
                printf("%s\n", groups[i].name);
            }
        }
    }

    for (size_t i = 0; i < group_count; i++) {
        free(groups[i].name);
        for (size_t j = 0; j < groups[i].desc_count; j++) {
            free(groups[i].descriptions[j]);
        }
        free(groups[i].descriptions);
    }
    free(groups);
    free(line);
    return 0;
}
