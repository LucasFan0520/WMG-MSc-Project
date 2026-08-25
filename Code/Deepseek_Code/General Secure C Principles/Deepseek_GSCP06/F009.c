// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Event {
    char *desc;
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

void add_event(const char *group, const char *desc) {
    Group *g = find_group(group);
    if (!g) {
        g = malloc(sizeof(Group));
        g->name = strdup(group);
        g->events = NULL;
        g->next = groups;
        groups = g;
    }
    Event *e = malloc(sizeof(Event));
    e->desc = strdup(desc);
    e->next = g->events;
    g->events = e;
}

void merge_groups(const char *a, const char *b, const char *newg) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    if (!ga || !gb) return;
    if (find_group(newg)) return; // avoid overwriting existing group
    Group *ng = malloc(sizeof(Group));
    ng->name = strdup(newg);
    ng->events = NULL;
    // copy events from ga and gb
    Event *e = ga->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        ne->desc = strdup(e->desc);
        ne->next = ng->events;
        ng->events = ne;
        e = e->next;
    }
    e = gb->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        ne->desc = strdup(e->desc);
        ne->next = ng->events;
        ng->events = ne;
        e = e->next;
    }
    ng->next = groups;
    groups = ng;
}

void delete_group(const char *name) {
    Group *g = find_group(name);
    if (!g) return;
    Group *cur = groups, *prev = NULL;
    while (cur) {
        if (cur == g) {
            if (prev) prev->next = cur->next;
            else groups = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    Event *e = g->events;
    while (e) {
        Event *tmp = e;
        e = e->next;
        free(tmp->desc);
        free(tmp);
    }
    free(g->name);
    free(g);
}

void count_group(const char *name) {
    Group *g = find_group(name);
    int c = 0;
    if (g) {
        Event *e = g->events;
        while (e) { c++; e = e->next; }
    }
    printf("%d\n", c);
}

void report_groups(void) {
    Group *g = groups;
    while (g) {
        Event *e = g->events;
        while (e) {
            printf("%s %s\n", g->name, e->desc);
            e = e->next;
        }
        g = g->next;
    }
}

void cleanup_groups(void) {
    Group *g = groups;
    while (g) {
        Group *tmp = g;
        g = g->next;
        Event *e = tmp->events;
        while (e) {
            Event *te = e;
            e = e->next;
            free(te->desc);
            free(te);
        }
        free(tmp->name);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], arg3[256], rest[2048];
        int n = sscanf(line, "%31s %255s %255s %255s", cmd, arg1, arg2, arg3);
        if (n < 2) continue;
        if (strcmp(cmd, "EVENT") == 0 && n >= 3) {
            char *desc = line + strlen(cmd) + 1 + strlen(arg1) + 1;
            add_event(arg1, desc);
        } else if (strcmp(cmd, "MERGE") == 0 && n == 4) {
            merge_groups(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            delete_group(arg1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            count_group(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    cleanup_groups();
    return 0;
}