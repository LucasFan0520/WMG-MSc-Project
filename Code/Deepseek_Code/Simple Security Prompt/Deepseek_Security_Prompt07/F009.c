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
    e->next = NULL;
    if (!g->events) {
        g->events = e;
    } else {
        Event *last = g->events;
        while (last->next) last = last->next;
        last->next = e;
    }
}

void merge_groups(const char *gA, const char *gB, const char *newG) {
    Group *a = find_group(gA);
    Group *b = find_group(gB);
    if (!a || !b || find_group(newG)) return;
    Group *ng = malloc(sizeof(Group));
    ng->name = strdup(newG);
    ng->events = NULL;
    Event *e = a->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        ne->desc = strdup(e->desc);
        ne->next = NULL;
        if (!ng->events) ng->events = ne;
        else {
            Event *last = ng->events;
            while (last->next) last = last->next;
            last->next = ne;
        }
        e = e->next;
    }
    e = b->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        ne->desc = strdup(e->desc);
        ne->next = NULL;
        if (!ng->events) ng->events = ne;
        else {
            Event *last = ng->events;
            while (last->next) last = last->next;
            last->next = ne;
        }
        e = e->next;
    }
    ng->next = groups;
    groups = ng;
}

void delete_group(const char *name) {
    Group *prev = NULL;
    Group *g = groups;
    while (g) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next;
            else groups = g->next;
            Event *e = g->events;
            while (e) {
                Event *next = e->next;
                free(e->desc);
                free(e);
                e = next;
            }
            free(g->name);
            free(g);
            return;
        }
        prev = g;
        g = g->next;
    }
}

void count_group(const char *name) {
    Group *g = find_group(name);
    int cnt = 0;
    if (g) {
        Event *e = g->events;
        while (e) { cnt++; e = e->next; }
    }
    printf("%d\n", cnt);
}

void report_groups() {
    Group *g = groups;
    while (g) {
        printf("%s:", g->name);
        Event *e = g->events;
        int first = 1;
        while (e) {
            if (!first) putchar(' ');
            for (char *s = e->desc; *s; s++) putchar(*s == ' ' ? '_' : *s);
            first = 0;
            e = e->next;
        }
        putchar('\n');
        g = g->next;
    }
}

void free_all() {
    Group *g = groups;
    while (g) {
        Group *nextg = g->next;
        Event *e = g->events;
        while (e) {
            Event *nexte = e->next;
            free(e->desc);
            free(e);
            e = nexte;
        }
        free(g->name);
        free(g);
        g = nextg;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "EVENT") == 0) {
            char grp[256], desc[4096];
            if (sscanf(rest, "%255s %[^\n]", grp, desc) == 2) {
                add_event(grp, desc);
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char ga[256], gb[256], ng[256];
            if (sscanf(rest, "%255s %255s %255s", ga, gb, ng) == 3) {
                merge_groups(ga, gb, ng);
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char grp[256];
            if (sscanf(rest, "%255s", grp) == 1) {
                delete_group(grp);
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char grp[256];
            if (sscanf(rest, "%255s", grp) == 1) {
                count_group(grp);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    free_all();
    return 0;
}