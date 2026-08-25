// F009.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char **events;
    int e_count;
    int e_capacity;
} Group;

int main(void) {
    Group *groups = NULL;
    int count = 0;
    int capacity = 0;
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
            while (*p == ' ') p++;
            char *gname = p;
            char *space = strchr(gname, ' ');
            if (!space) continue;
            *space = '\0';
            char *desc = space + 1;
            while (*desc == ' ') desc++;
            
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(groups[i].name, gname) == 0) {
                    found = i;
                    break;
                }
            }
            if (found == -1) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Group *tmp = realloc(groups, new_cap * sizeof(Group));
                    if (!tmp) return 1;
                    groups = tmp;
                    capacity = new_cap;
                }
                char *ng = strdup(gname);
                if (!ng) return 1;
                groups[count].name = ng;
                groups[count].events = NULL;
                groups[count].e_count = 0;
                groups[count].e_capacity = 0;
                found = count;
                count++;
            }
            Group *g = &groups[found];
            if (g->e_count >= g->e_capacity) {
                int new_ecap = g->e_capacity == 0 ? 4 : g->e_capacity * 2;
                char **tmp = realloc(g->events, new_ecap * sizeof(char *));
                if (!tmp) return 1;
                g->events = tmp;
                g->e_capacity = new_ecap;
            }
            char *nd = strdup(desc);
            if (!nd) return 1;
            g->events[g->e_count] = nd;
            g->e_count++;
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *ga = p;
            char *space1 = strchr(ga, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *gb = space1 + 1;
            while (*gb == ' ') gb++;
            char *space2 = strchr(gb, ' ');
            if (!space2) continue;
            *space2 = '\0';
            char *ng = space2 + 1;
            while (*ng == ' ') ng++;
            
            int fa = -1, fb = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(groups[i].name, ga) == 0) fa = i;
                if (strcmp(groups[i].name, gb) == 0) fb = i;
            }
            if (fa != -1 && fb != -1) {
                int total = groups[fa].e_count + groups[fb].e_count;
                char **ne = malloc(total * sizeof(char *));
                if (total > 0 && !ne) return 1;
                int idx = 0;
                for (int i = 0; i < groups[fa].e_count; i++) {
                    ne[idx] = strdup(groups[fa].events[i]);
                    if (!ne[idx]) return 1;
                    idx++;
                }
                for (int i = 0; i < groups[fb].e_count; i++) {
                    ne[idx] = strdup(groups[fb].events[i]);
                    if (!ne[idx]) return 1;
                    idx++;
                }
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Group *tmp = realloc(groups, new_cap * sizeof(Group));
                    if (!tmp) return 1;
                    groups = tmp;
                    capacity = new_cap;
                }
                char *nname = strdup(ng);
                if (!nname) return 1;
                groups[count].name = nname;
                groups[count].events = ne;
                groups[count].e_count = total;
                groups[count].e_capacity = total;
                count++;
            }
        } else if (strncmp(line, "DELETEGROUP ", 12) == 0) {
            char *gname = line + 12;
            while (*gname == ' ') gname++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(groups[i].name, gname) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(groups[found].name);
                for (int j = 0; j < groups[found].e_count; j++) free(groups[found].events[j]);
                free(groups[found].events);
                for (int i = found; i < count - 1; i++) {
                    groups[i] = groups[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *gname = line + 6;
            while (*gname == ' ') gname++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(groups[i].name, gname) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) printf("%d\n", groups[found].e_count);
            else printf("0\n");
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", groups[i].name);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(groups[i].name);
        for (int j = 0; j < groups[i].e_count; j++) free(groups[i].events[j]);
        free(groups[i].events);
    }
    free(groups);
    return 0;
}
