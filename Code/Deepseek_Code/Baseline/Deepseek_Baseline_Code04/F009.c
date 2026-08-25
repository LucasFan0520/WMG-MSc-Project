/* F009.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Event {
    char *description;
    struct Event *next;
} Event;

typedef struct Group {
    char *name;
    Event *events;
    struct Group *next;
} Group;

Group *groups = NULL;

Group *find_group(const char *name) {
    Group *g = groups;
    while (g) {
        if (strcmp(g->name, name) == 0) return g;
        g = g->next;
    }
    return NULL;
}

void add_event_to_group(Group *g, const char *desc) {
    Event *e = malloc(sizeof(Event));
    e->description = strdup(desc);
    e->next = g->events;
    g->events = e;
}

void create_group(const char *name) {
    if (find_group(name)) return;
    Group *g = malloc(sizeof(Group));
    g->name = strdup(name);
    g->events = NULL;
    g->next = groups;
    groups = g;
}

void merge_groups(const char *a, const char *b, const char *newg) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    if (!ga || !gb) return;
    Group *ng = malloc(sizeof(Group));
    ng->name = strdup(newg);
    ng->events = NULL;
    // copy events from both
    Event *e = ga->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        ne->description = strdup(e->description);
        ne->next = ng->events;
        ng->events = ne;
        e = e->next;
    }
    e = gb->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        ne->description = strdup(e->description);
        ne->next = ng->events;
        ng->events = ne;
        e = e->next;
    }
    ng->next = groups;
    groups = ng;
}

void delete_group(const char *name) {
    Group *prev = NULL, *cur = groups;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else groups = cur->next;
            // free events
            Event *e = cur->events;
            while (e) {
                Event *tmp = e;
                e = e->next;
                free(tmp->description);
                free(tmp);
            }
            free(cur->name);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

int count_events(Group *g) {
    int c = 0;
    Event *e = g->events;
    while (e) { c++; e = e->next; }
    return c;
}

void free_groups(void) {
    while (groups) {
        Group *tmp = groups;
        groups = groups->next;
        Event *e = tmp->events;
        while (e) {
            Event *te = e;
            e = e->next;
            free(te->description);
            free(te);
        }
        free(tmp->name);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char name[256], a[256], b[256], newg[256];
        int n = sscanf(line, "%s", cmd);
        if (n < 1) continue;
        if (strcmp(cmd, "EVENT") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *grp = p;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p++ = '\0';
            while (*p == ' ') p++;
            Group *g = find_group(grp);
            if (!g) {
                create_group(grp);
                g = find_group(grp);
            }
            if (g && *p) add_event_to_group(g, p);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *grpA = p;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p++ = '\0';
            while (*p == ' ') p++;
            char *grpB = p;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p++ = '\0';
            while (*p == ' ') p++;
            char *newGroup = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            merge_groups(grpA, grpB, newGroup);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *p = line + 12;
            while (*p == ' ') p++;
            char *grp = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            delete_group(grp);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *grp = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            Group *g = find_group(grp);
            if (g) printf("%d\n", count_events(g));
        } else if (strcmp(cmd, "REPORT") == 0) {
            Group *g = groups;
            while (g) {
                printf("%s\n", g->name);
                Event *e = g->events;
                while (e) {
                    printf("  %s\n", e->description);
                    e = e->next;
                }
                g = g->next;
            }
        }
    }
    free_groups();
    return 0;
}