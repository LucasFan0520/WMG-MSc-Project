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

Group *head = NULL;

Group* find_group(const char *name) {
    Group *g = head;
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
        free(tmp->desc);
        free(tmp);
    }
}

void free_group(Group *g) {
    free(g->name);
    free_events(g->events);
    free(g);
}

void add_event(const char *gname, const char *desc) {
    Group *g = find_group(gname);
    if (!g) {
        g = malloc(sizeof(Group));
        g->name = strdup(gname);
        g->events = NULL;
        g->next = NULL;
        if (!head) head = g;
        else {
            Group *last = head;
            while (last->next) last = last->next;
            last->next = g;
        }
    }
    Event *e = malloc(sizeof(Event));
    e->desc = strdup(desc);
    e->next = NULL;
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
    if (!ga || !gb || find_group(newg)) return;
    Group *ng = malloc(sizeof(Group));
    ng->name = strdup(newg);
    ng->events = NULL;
    // Copy events from ga
    Event *e = ga->events;
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
    // Copy events from gb
    e = gb->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        ne->desc = strdup(e->desc);
        ne->next = NULL;
        Event *last = ng->events;
        while (last->next) last = last->next;
        last->next = ne;
        e = e->next;
    }
    // Append to group list
    Group *last = head;
    while (last->next) last = last->next;
    last->next = ng;
}

void delete_group(const char *name) {
    Group *g = head, *prev = NULL;
    while (g) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next;
            else head = g->next;
            free_group(g);
            return;
        }
        prev = g;
        g = g->next;
    }
}

void count_group(const char *name) {
    Group *g = find_group(name);
    if (!g) { printf("0\n"); return; }
    int cnt = 0;
    Event *e = g->events;
    while (e) { cnt++; e = e->next; }
    printf("%d\n", cnt);
}

void report() {
    Group *g = head;
    while (g) {
        printf("%s", g->name);
        Event *e = g->events;
        while (e) {
            printf(" %s", e->desc);
            e = e->next;
        }
        putchar('\n');
        g = g->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "EVENT") == 0) {
            char *group = strtok(NULL, " ");
            char *desc = strtok(NULL, "");
            if (group && desc) add_event(group, desc);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *a = strtok(NULL, " ");
            char *b = strtok(NULL, " ");
            char *newg = strtok(NULL, " ");
            if (a && b && newg) merge_groups(a, b, newg);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *group = strtok(NULL, " ");
            if (group) delete_group(group);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *group = strtok(NULL, " ");
            if (group) count_group(group);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    while (head) {
        Group *tmp = head;
        head = head->next;
        free_group(tmp);
    }
    return 0;
}