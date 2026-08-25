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
Group *tail = NULL;

Group *find_group(const char *name) {
    Group *g = head;
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
        g->name = malloc(strlen(group)+1);
        strcpy(g->name, group);
        g->events = NULL;
        g->next = NULL;
        if (tail) tail->next = g;
        else head = g;
        tail = g;
    }
    Event *e = malloc(sizeof(Event));
    e->desc = malloc(strlen(desc)+1);
    strcpy(e->desc, desc);
    e->next = g->events;
    g->events = e;
}

void merge_groups(const char *a, const char *b, const char *new) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    if (!ga || !gb || find_group(new)) return;
    Group *gn = malloc(sizeof(Group));
    gn->name = malloc(strlen(new)+1);
    strcpy(gn->name, new);
    gn->events = NULL;
    gn->next = NULL;
    // Copy events from ga
    Event *e = ga->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        ne->desc = malloc(strlen(e->desc)+1);
        strcpy(ne->desc, e->desc);
        ne->next = gn->events;
        gn->events = ne;
        e = e->next;
    }
    // Copy events from gb
    e = gb->events;
    while (e) {
        Event *ne = malloc(sizeof(Event));
        ne->desc = malloc(strlen(e->desc)+1);
        strcpy(ne->desc, e->desc);
        ne->next = gn->events;
        gn->events = ne;
        e = e->next;
    }
    if (tail) tail->next = gn;
    else head = gn;
    tail = gn;
}

void delete_group(const char *name) {
    Group *prev = NULL;
    Group *g = head;
    while (g) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next;
            else head = g->next;
            if (g == tail) tail = prev;
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
    int cnt = 0;
    Event *e = g->events;
    while (e) { cnt++; e = e->next; }
    printf("%d\n", cnt);
}

void report_groups(void) {
    Group *g = head;
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
    Group *g = head;
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
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "EVENT") == 0) {
            char *group = strtok(rest, " ");
            if (!group) continue;
            char *desc = strtok(NULL, "");
            if (!desc) desc = "";
            add_event(group, desc);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *a = strtok(rest, " ");
            char *b = strtok(NULL, " ");
            char *new = strtok(NULL, " ");
            if (a && b && new) merge_groups(a, b, new);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *name = strtok(rest, " ");
            if (name) delete_group(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *name = strtok(rest, " ");
            if (name) count_group(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    free_all();
    return 0;
}