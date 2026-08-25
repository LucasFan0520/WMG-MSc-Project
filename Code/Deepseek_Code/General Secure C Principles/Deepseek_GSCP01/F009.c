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

void add_event(const char *group, const char *description) {
    Group *g = find_group(group);
    if (!g) {
        g = malloc(sizeof(Group));
        g->name = strdup(group);
        g->events = NULL;
        g->next = groups;
        groups = g;
    }
    Event *e = malloc(sizeof(Event));
    e->description = strdup(description);
    e->next = g->events;
    g->events = e;
}

void merge_groups(const char *a, const char *b, const char *new) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    if (!ga || !gb) return;
    Group *gn = find_group(new);
    if (gn) return;
    gn = malloc(sizeof(Group));
    gn->name = strdup(new);
    gn->events = NULL;
    Event *e = ga->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        ne->description = strdup(e->description);
        ne->next = gn->events;
        gn->events = ne;
        e = e->next;
    }
    e = gb->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        ne->description = strdup(e->description);
        ne->next = gn->events;
        gn->events = ne;
        e = e->next;
    }
    gn->next = groups;
    groups = gn;
}

void delete_group(const char *name) {
    Group *g = groups, *prev = NULL;
    while (g && strcmp(g->name, name) != 0) {
        prev = g;
        g = g->next;
    }
    if (!g) return;
    if (prev) prev->next = g->next;
    else groups = g->next;
    free(g->name);
    Event *e = g->events;
    while (e) {
        Event *next = e->next;
        free(e->description);
        free(e);
        e = next;
    }
    free(g);
}

void count_events(const char *name) {
    Group *g = find_group(name);
    if (!g) { printf("0\n"); return; }
    int cnt = 0;
    Event *e = g->events;
    while (e) { cnt++; e = e->next; }
    printf("%d\n", cnt);
}

void report_all(void) {
    Group *g = groups;
    while (g) {
        printf("%s\n", g->name);
        Event *e = g->events;
        while (e) {
            for (char *c = e->description; *c; c++) {
                putchar(*c == ' ' ? '_' : *c);
            }
            putchar('\n');
            e = e->next;
        }
        g = g->next;
    }
}

void free_all(void) {
    Group *g = groups;
    while (g) {
        Group *nextg = g->next;
        free(g->name);
        Event *e = g->events;
        while (e) {
            Event *nexte = e->next;
            free(e->description);
            free(e);
            e = nexte;
        }
        free(g);
        g = nextg;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "EVENT") == 0) {
            char group[256];
            char *p = line + 6;
            if (sscanf(p, "%255s", group) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            add_event(group, p);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char a[256], b[256], new[256];
            if (sscanf(line + 6, "%255s %255s %255s", a, b, new) == 3)
                merge_groups(a, b, new);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *p = line + 13;
            while (*p == ' ') p++;
            delete_group(p);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            count_events(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    free(line);
    return 0;
}