// filename: F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct EventNode {
    char *description;
    struct EventNode *next;
} EventNode;

typedef struct Group {
    char *name;
    EventNode *events;
    struct Group *next;
} Group;

static Group *groups = NULL;

static void free_events(EventNode *e) {
    while (e) {
        EventNode *tmp = e;
        e = e->next;
        free(tmp->description);
        free(tmp);
    }
}

static void cleanup(void) {
    while (groups) {
        Group *tmp = groups;
        groups = groups->next;
        free(tmp->name);
        free_events(tmp->events);
        free(tmp);
    }
}

static Group *find_group(const char *name) {
    for (Group *g = groups; g; g = g->next)
        if (strcmp(g->name, name) == 0)
            return g;
    return NULL;
}

static void add_event(const char *group, const char *desc) {
    Group *g = find_group(group);
    if (!g) {
        g = malloc(sizeof(Group));
        if (!g) return;
        g->name = strdup(group);
        if (!g->name) { free(g); return; }
        g->events = NULL;
        g->next = groups;
        groups = g;
    }
    EventNode *e = malloc(sizeof(EventNode));
    if (!e) return;
    e->description = strdup(desc);
    if (!e->description) { free(e); return; }
    e->next = g->events;
    g->events = e;
}

static void merge_groups(const char *ga, const char *gb, const char *newg) {
    Group *a = find_group(ga);
    Group *b = find_group(gb);
    if (!a || !b) return;
    Group *ng = malloc(sizeof(Group));
    if (!ng) return;
    ng->name = strdup(newg);
    if (!ng->name) { free(ng); return; }
    ng->events = NULL;
    // copy all events from a and b
    for (EventNode *e = a->events; e; e = e->next) {
        EventNode *ne = malloc(sizeof(EventNode));
        if (!ne) { free_events(ng->events); free(ng->name); free(ng); return; }
        ne->description = strdup(e->description);
        if (!ne->description) { free(ne); free_events(ng->events); free(ng->name); free(ng); return; }
        ne->next = ng->events;
        ng->events = ne;
    }
    for (EventNode *e = b->events; e; e = e->next) {
        EventNode *ne = malloc(sizeof(EventNode));
        if (!ne) { free_events(ng->events); free(ng->name); free(ng); return; }
        ne->description = strdup(e->description);
        if (!ne->description) { free(ne); free_events(ng->events); free(ng->name); free(ng); return; }
        ne->next = ng->events;
        ng->events = ne;
    }
    ng->next = groups;
    groups = ng;
}

static void delete_group(const char *name) {
    Group *prev = NULL;
    for (Group *g = groups; g; g = g->next) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next;
            else groups = g->next;
            free(g->name);
            free_events(g->events);
            free(g);
            return;
        }
        prev = g;
    }
}

static void count_events(const char *group) {
    Group *g = find_group(group);
    int cnt = 0;
    if (g) for (EventNode *e = g->events; e; e = e->next) ++cnt;
    printf("%d\n", cnt);
}

static void report_groups(void) {
    for (Group *g = groups; g; g = g->next) {
        printf("%s:", g->name);
        for (EventNode *e = g->events; e; e = e->next)
            printf(" %s", e->description);
        putchar('\n');
    }
}

int main(void) {
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg3 = p;
        if (strcmp(cmd, "EVENT") == 0) {
            if (*arg1) add_event(arg1, arg2);
        } else if (strcmp(cmd, "MERGE") == 0) {
            if (*arg1 && *arg2 && *arg3) merge_groups(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            if (*arg1) delete_group(arg1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (*arg1) count_events(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    free(line);
    return 0;
}