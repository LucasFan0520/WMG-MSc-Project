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
        if (!g) return;
        g->name = strdup(group);
        g->events = NULL;
        g->next = NULL;
        if (!g->name) { free(g); return; }
        if (!groups) groups = g;
        else {
            Group *last = groups;
            while (last->next) last = last->next;
            last->next = g;
        }
    }
    Event *e = malloc(sizeof(Event));
    if (!e) return;
    e->desc = strdup(desc);
    e->next = NULL;
    if (!e->desc) { free(e); return; }
    if (!g->events) g->events = e;
    else {
        Event *last = g->events;
        while (last->next) last = last->next;
        last->next = e;
    }
}

void merge_groups(const char *a, const char *b, const char *newg) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    if (!ga || !gb) return;
    if (find_group(newg)) return;
    Group *ng = malloc(sizeof(Group));
    if (!ng) return;
    ng->name = strdup(newg);
    ng->events = NULL;
    ng->next = NULL;
    if (!ng->name) { free(ng); return; }
    Event *e = ga->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        if (!ne) { /* cleanup? */ break; }
        ne->desc = strdup(e->desc);
        ne->next = NULL;
        if (!ne->desc) { free(ne); break; }
        if (!ng->events) ng->events = ne;
        else {
            Event *last = ng->events;
            while (last->next) last = last->next;
            last->next = ne;
        }
        e = e->next;
    }
    e = gb->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        if (!ne) break;
        ne->desc = strdup(e->desc);
        ne->next = NULL;
        if (!ne->desc) { free(ne); break; }
        if (!ng->events) ng->events = ne;
        else {
            Event *last = ng->events;
            while (last->next) last = last->next;
            last->next = ne;
        }
        e = e->next;
    }
    if (!groups) groups = ng;
    else {
        Group *last = groups;
        while (last->next) last = last->next;
        last->next = ng;
    }
}

void delete_group(const char *name) {
    Group *g = groups, *prev = NULL;
    while (g) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next;
            else groups = g->next;
            free(g->name);
            Event *e = g->events;
            while (e) {
                Event *next = e->next;
                free(e->desc);
                free(e);
                e = next;
            }
            free(g);
            return;
        }
        prev = g;
        g = g->next;
    }
}

void count_group(const char *name) {
    Group *g = find_group(name);
    if (!g) { printf("0\n"); return; }
    int c = 0;
    Event *e = g->events;
    while (e) { c++; e = e->next; }
    printf("%d\n", c);
}

void report_groups(void) {
    Group *g = groups;
    while (g) {
        printf("%s\n", g->name);
        Event *e = g->events;
        while (e) {
            printf("  %s\n", e->desc);
            e = e->next;
        }
        g = g->next;
    }
}

void free_all(void) {
    Group *g = groups;
    while (g) {
        Group *next = g->next;
        free(g->name);
        Event *e = g->events;
        while (e) {
            Event *enext = e->next;
            free(e->desc);
            free(e);
            e = enext;
        }
        free(g);
        g = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], g1[256], g2[256], g3[256], desc[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "EVENT") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", g1, desc) == 2)
                add_event(g1, desc);
        } else if (strcmp(cmd, "MERGE") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", g1, g2, g3) == 3)
                merge_groups(g1, g2, g3);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            if (sscanf(line, "%*s %255s", g1) == 1)
                delete_group(g1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(line, "%*s %255s", g1) == 1)
                count_group(g1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    free(line);
    free_all();
    return 0;
}