// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Event {
    char *desc;
    struct Event *next;
} Event;

typedef struct Group {
    char *name;
    Event *events;
    int count;
    struct Group *next;
} Group;

Group *groups = NULL;

Group* find_group(const char *name) {
    for (Group *g = groups; g; g = g->next)
        if (strcmp(g->name, name) == 0) return g;
    return NULL;
}

void add_event_to_group(Group *g, const char *desc) {
    Event *e = malloc(sizeof(Event));
    if (!e) return;
    e->desc = strdup(desc);
    e->next = g->events;
    g->events = e;
    g->count++;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "EVENT") == 0) {
            char group[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", group, &n) != 1) continue;
            char *desc = rest + n;
            while (*desc == ' ') desc++;
            Group *g = find_group(group);
            if (!g) {
                g = malloc(sizeof(Group));
                if (!g) continue;
                g->name = strdup(group);
                g->events = NULL;
                g->count = 0;
                g->next = groups;
                groups = g;
            }
            add_event_to_group(g, desc);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char gA[256], gB[256], newG[256];
            if (sscanf(rest, "%255s %255s %255s", gA, gB, newG) != 3) continue;
            Group *A = find_group(gA);
            Group *B = find_group(gB);
            if (!A || !B) continue;
            
            Group *C = find_group(newG);
            if (!C) {
                C = malloc(sizeof(Group));
                if (!C) continue;
                C->name = strdup(newG);
                C->events = NULL;
                C->count = 0;
                C->next = groups;
                groups = C;
            }
            
            for (Event *e = A->events; e; e = e->next) add_event_to_group(C, e->desc);
            for (Event *e = B->events; e; e = e->next) add_event_to_group(C, e->desc);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Group *prev = NULL;
            for (Group *g = groups; g; prev = g, g = g->next) {
                if (strcmp(g->name, name) == 0) {
                    if (prev) prev->next = g->next;
                    else groups = g->next;
                    while (g->events) {
                        Event *e = g->events;
                        g->events = e->next;
                        free(e->desc);
                        free(e);
                    }
                    free(g->name);
                    free(g);
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Group *g = find_group(name);
            printf("%d\n", g ? g->count : 0);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Group *g = groups; g; g = g->next)
                printf("%s %d\n", g->name, g->count);
        }
    }
    while (groups) {
        Group *g = groups;
        groups = groups->next;
        while (g->events) {
            Event *e = g->events;
            g->events = e->next;
            free(e->desc);
            free(e);
        }
        free(g->name);
        free(g);
    }
    return 0;
}