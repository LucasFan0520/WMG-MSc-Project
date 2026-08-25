/* F009.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct event {
    char *desc;
    struct event *next;
} Event;

typedef struct group {
    char *name;
    Event *events;
    struct group *next;
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

void add_event(const char *gname, const char *desc) {
    Group *g = find_group(gname);
    if (!g) {
        g = malloc(sizeof(Group));
        g->name = strdup(gname);
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
    Group *ga = find_group(a), *gb = find_group(b);
    if (!ga || !gb || find_group(newg)) return;
    Group *ng = malloc(sizeof(Group));
    ng->name = strdup(newg);
    ng->events = NULL;
    // copy events from ga, then gb (order arbitrary)
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
    Group *g = groups, *prev = NULL;
    while (g) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next;
            else groups = g->next;
            Event *e = g->events;
            while (e) { Event *tmp = e; e = e->next; free(tmp->desc); free(tmp); }
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
    Event *e = g ? g->events : NULL;
    while (e) { cnt++; e = e->next; }
    printf("%d\n", cnt);
}

void report() {
    Group *g = groups;
    while (g) {
        printf("%s\n", g->name);
        Event *e = g->events;
        while (e) {
            for (char *c = e->desc; *c; c++) putchar(*c == ' ' ? '_' : *c);
            putchar('\n');
            e = e->next;
        }
        g = g->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], g1[256], g2[256], ng[256], desc[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "REPORT") == 0) { report(); continue; }
        if (strcmp(cmd, "EVENT") == 0) {
            if (sscanf(line, "%*s %255s", g1) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(g1) + 1;
            if (*rest == ' ') rest++;
            add_event(g1, rest);
        } else if (strcmp(cmd, "MERGE") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", g1, g2, ng) == 3)
                merge_groups(g1, g2, ng);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            if (sscanf(line, "%*s %255s", g1) == 1) delete_group(g1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(line, "%*s %255s", g1) == 1) count_group(g1);
        }
    }
    return 0;
}