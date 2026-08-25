// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_GROUP 64
#define MAX_DESC 4096

typedef struct Event {
    char desc[MAX_DESC];
    struct Event *next;
} Event;

typedef struct Group {
    char name[MAX_GROUP];
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

void free_events(Event *e) {
    while (e) {
        Event *tmp = e;
        e = e->next;
        free(tmp);
    }
}

void free_all() {
    Group *g = groups;
    while (g) {
        Group *tmp = g;
        free_events(g->events);
        g = g->next;
        free(tmp);
    }
    groups = NULL;
}

void handle_event(char *line) {
    char group[MAX_GROUP], desc[MAX_DESC];
    int n = sscanf(line, "EVENT %63s %4095[^\n]", group, desc);
    if (n < 2) return;
    Group *g = find_group(group);
    if (!g) {
        g = malloc(sizeof(Group));
        if (!g) return;
        strcpy(g->name, group);
        g->events = NULL;
        g->next = groups;
        groups = g;
    }
    Event *e = malloc(sizeof(Event));
    if (!e) return;
    strcpy(e->desc, desc);
    e->next = g->events;
    g->events = e;
}

void handle_merge(char *line) {
    char ga[MAX_GROUP], gb[MAX_GROUP], ng[MAX_GROUP];
    if (sscanf(line, "MERGE %63s %63s %63s", ga, gb, ng) != 3) return;
    Group *a = find_group(ga);
    Group *b = find_group(gb);
    if (!a || !b) return;
    if (find_group(ng)) return;
    Group *newg = malloc(sizeof(Group));
    if (!newg) return;
    strcpy(newg->name, ng);
    newg->events = NULL;
    // copy events from a
    Event *e = a->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        if (!ne) { free_events(newg->events); free(newg); return; }
        strcpy(ne->desc, e->desc);
        ne->next = newg->events;
        newg->events = ne;
        e = e->next;
    }
    e = b->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        if (!ne) { free_events(newg->events); free(newg); return; }
        strcpy(ne->desc, e->desc);
        ne->next = newg->events;
        newg->events = ne;
        e = e->next;
    }
    newg->next = groups;
    groups = newg;
}

void handle_deletegroup(char *line) {
    char name[MAX_GROUP];
    if (sscanf(line, "DELETEGROUP %63s", name) != 1) return;
    Group *g = groups, *prev = NULL;
    while (g) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next;
            else groups = g->next;
            free_events(g->events);
            free(g);
            return;
        }
        prev = g;
        g = g->next;
    }
}

void handle_count(char *line) {
    char name[MAX_GROUP];
    if (sscanf(line, "COUNT %63s", name) != 1) return;
    Group *g = find_group(name);
    if (!g) { printf("0\n"); return; }
    int cnt = 0;
    Event *e = g->events;
    while (e) { cnt++; e = e->next; }
    printf("%d\n", cnt);
}

void handle_report() {
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

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "EVENT ", 6) == 0) handle_event(line);
        else if (strncmp(line, "MERGE ", 6) == 0) handle_merge(line);
        else if (strncmp(line, "DELETEGROUP ", 12) == 0) handle_deletegroup(line);
        else if (strncmp(line, "COUNT ", 6) == 0) handle_count(line);
        else if (strcmp(line, "REPORT") == 0) handle_report();
    }
    free_all();
    return 0;
}