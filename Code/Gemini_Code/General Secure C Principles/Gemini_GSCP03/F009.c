// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *desc;
} Event;

typedef struct {
    char *name;
    Event *events;
    int count;
    int cap;
} Group;

int main() {
    Group *groups = NULL;
    int group_count = 0;
    int group_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < group_count; i++) {
                printf("%s\n", groups[i].name);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "EVENT") == 0) {
            char *group = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *desc = p;
            int g_idx = -1;
            for (int i = 0; i < group_count; i++) {
                if (strcmp(groups[i].name, group) == 0) {
                    g_idx = i;
                    break;
                }
            }
            if (g_idx == -1) {
                if (group_count >= group_cap) {
                    group_cap = group_cap == 0 ? 16 : group_cap * 2;
                    Group *nb = realloc(groups, group_cap * sizeof(Group));
                    if (nb) groups = nb;
                }
                groups[group_count].name = strdup(group);
                groups[group_count].events = NULL;
                groups[group_count].count = 0;
                groups[group_count].cap = 0;
                g_idx = group_count;
                group_count++;
            }
            Group *g = &groups[g_idx];
            if (g->count >= g->cap) {
                g->cap = g->cap == 0 ? 16 : g->cap * 2;
                Event *nb = realloc(g->events, g->cap * sizeof(Event));
                if (nb) g->events = nb;
            }
            g->events[g->count].desc = strdup(desc);
            g->count++;;
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *groupA = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *groupB = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *newGroup = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            int idxA = -1, idxB = -1;
            for (int i = 0; i < group_count; i++) {
                if (strcmp(groups[i].name, groupA) == 0) idxA = i;
                if (strcmp(groups[i].name, groupB) == 0) idxB = i;
            }
            if (idxA != -1 && idxB != -1) {
                if (group_count >= group_cap) {
                    group_cap = group_cap == 0 ? 16 : group_cap * 2;
                    Group *nb = realloc(groups, group_cap * sizeof(Group));
                    if (nb) groups = nb;
                }
                groups[group_count].name = strdup(newGroup);
                groups[group_count].count = 0;
                groups[group_count].cap = groups[idxA].count + groups[idxB].count;
                groups[group_count].events = malloc(groups[group_count].cap * sizeof(Event));
                for (int j = 0; j < groups[idxA].count; j++) {
                    groups[group_count].events[groups[group_count].count].desc = strdup(groups[idxA].events[j].desc);
                    groups[group_count].count++;
                }
                for (int j = 0; j < groups[idxB].count; j++) {
                    groups[group_count].events[groups[group_count].count].desc = strdup(groups[idxB].events[j].desc);
                    groups[group_count].count++;
                }
                group_count++;
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *group = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (int i = 0; i < group_count; i++) {
                if (strcmp(groups[i].name, group) == 0) {
                    free(groups[i].name);
                    for (int j = 0; j < groups[i].count; j++) {
                        free(groups[i].events[j].desc);
                    }
                    free(groups[i].events);
                    for (int j = i; j < group_count - 1; j++) {
                        groups[j] = groups[j + 1];
                    }
                    group_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *group = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            int val = 0;
            for (int i = 0; i < group_count; i++) {
                if (strcmp(groups[i].name, group) == 0) {
                    val = groups[i].count;
                    break;
                }
            }
            printf("%d\n", val);
        }
        free(line);
    }
    for (int i = 0; i < group_count; i++) {
        free(groups[i].name);
        for (int j = 0; j < groups[i].count; j++) {
            free(groups[i].events[j].desc);
        }
        free(groups[i].events);
    }
    free(groups);
    return 0;
}
