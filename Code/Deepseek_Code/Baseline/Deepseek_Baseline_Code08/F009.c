// F009.c
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

Group* find_group(const char *name) {
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
        g->next = NULL;
        if (!groups) groups = g;
        else {
            Group *cur = groups;
            while (cur->next) cur = cur->next;
            cur->next = g;
        }
    }
    Event *e = malloc(sizeof(Event));
    e->description = strdup(desc);
    e->next = NULL;
    if (!g->events) g->events = e;
    else {
        Event *cur = g->events;
        while (cur->next) cur = cur->next;
        cur->next = e;
    }
}

void merge_groups(const char *a, const char *b, const char *newg) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    if (!ga || !gb || find_group(newg)) return;
    Group *ng = malloc(sizeof(Group));
    ng->name = strdup(newg);
    ng->events = NULL;
    ng->next = NULL;
    // copy events from ga
    Event *e = ga->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        ne->description = strdup(e->description);
        ne->next = NULL;
        if (!ng->events) ng->events = ne;
        else {
            Event *cur = ng->events;
            while (cur->next) cur = cur->next;
            cur->next = ne;
        }
        e = e->next;
    }
    // copy from gb
    e = gb->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        ne->description = strdup(e->description);
        ne->next = NULL;
        if (!ng->events) ng->events = ne;
        else {
            Event *cur = ng->events;
            while (cur->next) cur = cur->next;
            cur->next = ne;
        }
        e = e->next;
    }
    // add new group
    if (!groups) groups = ng;
    else {
        Group *cur = groups;
        while (cur->next) cur = cur->next;
        cur->next = ng;
    }
}

void delete_group(const char *name) {
    Group *prev = NULL, *cur = groups;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else groups = cur->next;
            free(cur->name);
            Event *e = cur->events;
            while (e) {
                Event *tmp = e;
                e = e->next;
                free(tmp->description);
                free(tmp);
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

int count_events(const char *name) {
    Group *g = find_group(name);
    if (!g) return 0;
    int cnt = 0;
    Event *e = g->events;
    while (e) { cnt++; e = e->next; }
    return cnt;
}

void free_all() {
    Group *cur = groups;
    while (cur) {
        Group *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        Event *e = tmp->events;
        while (e) {
            Event *te = e;
            e = e->next;
            free(te->description);
            free(te);
        }
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "EVENT") == 0) {
            char *group = strtok(NULL, " ");
            if (!group) continue;
            char *desc = strtok(NULL, "");
            if (!desc) desc = "";
            add_event(group, desc);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *a = strtok(NULL, " ");
            if (!a) continue;
            char *b = strtok(NULL, " ");
            if (!b) continue;
            char *newg = strtok(NULL, " ");
            if (!newg) continue;
            merge_groups(a, b, newg);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *group = strtok(NULL, " ");
            if (group) delete_group(group);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *group = strtok(NULL, " ");
            if (!group) continue;
            printf("%d\n", count_events(group));
        } else if (strcmp(cmd, "REPORT") == 0) {
            Group *g = groups;
            while (g) {
                printf("%s: ", g->name);
                Event *e = g->events;
                int first = 1;
                while (e) {
                    if (!first) printf(", ");
                    printf("%s", e->description);
                    first = 0;
                    e = e->next;
                }
                printf("\n");
                g = g->next;
            }
        }
    }
    free_all();
    return 0;
}